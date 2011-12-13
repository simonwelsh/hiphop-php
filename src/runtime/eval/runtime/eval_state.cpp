/*
   +----------------------------------------------------------------------+
   | HipHop for PHP                                                       |
   +----------------------------------------------------------------------+
   | Copyright (c) 2010- Facebook, Inc. (http://www.facebook.com)         |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
*/
#include <runtime/eval/ast/expression.h>
#include <runtime/eval/runtime/eval_state.h>
#include <runtime/eval/base/function.h>
#include <runtime/eval/ast/function_statement.h>
#include <runtime/eval/ast/class_statement.h>
#include <runtime/eval/runtime/file_repository.h>
#include <runtime/base/util/request_local.h>
#include <runtime/base/complex_types.h>
#include <runtime/base/array/array_iterator.h>
#include <runtime/base/runtime_option.h>
#include <runtime/eval/ext/ext.h>
#include <util/util.h>
#include <runtime/base/source_info.h>
#include <runtime/eval/parser/parser.h>
#include <runtime/eval/runtime/eval_object_data.h>
#include <runtime/eval/ast/method_statement.h>
#include <runtime/eval/eval.h>

namespace HPHP {
namespace Eval {
using namespace std;
///////////////////////////////////////////////////////////////////////////////

static StaticString s___get("__get");
static StaticString s___set("__set");
static StaticString s___isset("__isset");
static StaticString s___unset("__unset");
static StaticString s___call("__call");
static StaticString s___callstatic("__callstatic");

void CodeContainer::release() {
  delete this;
}

StringCodeContainer::StringCodeContainer(StatementPtr s) : m_s(s) {}
StringCodeContainer::~StringCodeContainer() {}

void ClassEvalState::init(const ClassStatement *cls) {
  m_class = cls;
}

const MethodStatement *ClassEvalState::getMethod(CStrRef m, int &access) {
  if (!m_doneSemanticCheck) semanticCheck();
  MethodTable::const_iterator it = m_methodTable.find(m);
  if (it == m_methodTable.end()) return NULL;
  access = it->second.second;
  return it->second.first;
}

const MethodStatement *ClassEvalState::getTraitMethod(CStrRef m,
                                                      int &access) {
  MethodTable::const_iterator it = m_traitMethodTable.find(m);
  if (it == m_traitMethodTable.end()) return NULL;
  access = it->second.second;
  return it->second.first;
}

void ClassEvalState::semanticCheck() {
  if (!m_doneSemanticCheck) {
    m_class->loadMethodTable(*this);
    m_class->semanticCheck(NULL);
    m_doneSemanticCheck = true;
  }
}

void ClassEvalState::initializeInstance() {
  if (!m_initializedInstance) {
    semanticCheck();
    m_initializedInstance = true;
  }
}

void ClassEvalState::initializeStatics() {
  if (!m_initializedStatics) {
    semanticCheck();
    m_class->initializeStatics(*this, m_statics);
    m_initializedStatics = true;
  }
}

void ClassEvalState::implementTrait(const ClassStatement *trait) {
  bool ignore = false;
  for (unsigned int i = 0; i < m_traits.size(); i++) {
    if (m_traits[i] == trait) {
      ignore = true;
      break;
    }
  }
  if (!ignore) m_traits.push_back(trait);
}

void ClassEvalState::compileExcludeTable(
  StringISet &excludeTable,
  const std::vector<TraitPrecedence> &traitPrecedences,
  const ClassStatement *trait) {
  for (unsigned int i = 0; i < traitPrecedences.size(); i++) {
    const TraitPrecedence &traitPrecedence = traitPrecedences[i]; 
    for (unsigned int j = 0;
         j < traitPrecedence.m_excludeFromClasses.size(); j++) {
      if (traitPrecedence.m_excludeFromClasses[j]->getClass() == trait) {
        if (!excludeTable.insert(traitPrecedence.m_method).second) {
          raise_error("Failed to evaluate a trait precedence (%s). "
                      "Method of trait %s was defined to be excluded "
                      "multiple times",
                      traitPrecedence.m_method.c_str(), trait->name().c_str());
        }
      }
    }
  }
}

void ClassEvalState::copyTraitMethodTable(
  MethodTable &methodTable,
  const ClassStatement *trait,
  const std::vector<TraitAlias> &traitAliases,
  const StringISet &excludeTable) {
  String traitName = trait->name();
  ClassEvalState *ce = RequestEvalState::findClassState(traitName);
  ASSERT(ce);
  ce->semanticCheck();
  MethodTable &traitMethods = ce->getMethodTable();
  for (MethodTable::const_iterator cit = traitMethods.begin();
       cit != traitMethods.end(); cit++) {
    String traitMethodName = cit->first;
    const MethodStatement *traitMethod = cit->second.first;
    for (unsigned int j = 0; j < traitAliases.size(); j++) {
      const TraitAlias &traitAlias = traitAliases[j];
      if ((traitAlias.m_trait.empty() || traitAlias.m_trait == traitName) &&
          traitAlias.m_method == traitMethodName) {
        pair<const MethodStatement *, int> &p =
          methodTable[traitAlias.m_alias.c_str()];
        p.first = traitMethod;
        p.second = traitAlias.m_modifiers ? traitAlias.m_modifiers
                                          : traitMethod->getModifiers();
      }
    }
    if (excludeTable.empty() ||
        excludeTable.find(traitMethodName) == excludeTable.end()) {
      methodTable.insert(MethodTable::value_type(
        traitMethodName.c_str(),
        std::pair<const MethodStatement*, int>(
          traitMethod, traitMethod->getModifiers())));
    }
  }
}

void ClassEvalState::mergeTraitMethods(const MethodTable &currentTable,
  unsigned int current, unsigned count,
  std::vector<MethodTable> &methodTables,
  MethodTable &resultTable) {
  for (MethodTable::const_iterator it = currentTable.begin();
       it != currentTable.end(); ++it) {
    int collisions = 0;
    bool abstract_solved = false;
    for (unsigned int i = 0; i < methodTables.size(); i++) {
      if (i == current) continue;
      ClassEvalState::MethodTable::const_iterator mit = 
        methodTables[i].find(it->first);
      if (mit != methodTables[i].end()) {
        // if it is an abstract method, there is no collision
        if (mit->second.second & ClassStatement::Abstract) {
          // we can safely free and remove it from methodTables[i]
          methodTables[i].erase(it->first);
        } else {
          // if it is not an abstract method, there is still no collision
          // if it->second is an abstract method
          if (it->second.second & ClassStatement::Abstract) {
            // just mark as solved, will be added if its own trait is processed
            abstract_solved = true;
          } else {
            // we have a collision of non-abstract methods
            collisions++;
            methodTables[i].erase(it->first);
          }
        }
      }
    }
    if (collisions) {
      ClassEvalState::MethodTable::const_iterator mit = 
        m_methodTable.find(it->first);
      if (mit == m_methodTable.end() ||
          (mit->second.first->getClass() != m_class)) {
        raise_error("Trait method %s has not been applied, because there are "
                    "collisions with other trait methods on %s",
                    it->first.c_str(), m_class->name().c_str());
      }
    } else if (!abstract_solved) {
      if (resultTable.find(it->first) != resultTable.end()) {
        raise_error("Trait method %s has not been applied, because failure "
                    "occured during updating resulting trait method table",
                    it->first.c_str());
      } else {
        resultTable[it->first] = it->second;
      }
    }
  }
}

void ClassEvalState::mergeTraitMethodsToClass(const MethodTable &resultTable,
  const ClassStatement *cls) {
  for (MethodTable::const_iterator it = resultTable.begin();
       it != resultTable.end(); ++it) {
    ClassEvalState::MethodTable::const_iterator mit =
      m_methodTable.find(it->first);
    if (mit == m_methodTable.end() ||
        (mit->second.first->getClass() != m_class) &&
         !it->second.first->isAbstract()) {
      m_methodTable[it->first] = it->second;
      if (m_class == cls) {
        m_traitMethodTable[it->first] = it->second;
        if (strcmp(it->first, "__construct") == 0) {
          m_constructor = it->second.first;
        }
      }
      CStrRef n = it->second.first->name();
      if (n->isame(s___get.get())) setAttributes(ObjectData::UseGet);
      if (n->isame(s___set.get())) setAttributes(ObjectData::UseSet);
      if (n->isame(s___isset.get())) setAttributes(ObjectData::UseIsset);
      if (n->isame(s___unset.get())) setAttributes(ObjectData::UseUnset);
      if (n->isame(s___call.get())) setAttributes(ObjectData::HasCall);
      if (n->isame(s___callstatic.get())) {
        setAttributes(ObjectData::HasCallStatic);
      }
    }
  }
}

void ClassEvalState::fiberInit(ClassEvalState &oces,
                               FiberReferenceMap &refMap) {
  m_class = oces.m_class;
  if (oces.m_constructor) {
    m_constructor = oces.m_constructor;
  }

  for (MethodTable::const_iterator it = oces.m_methodTable.begin();
       it != oces.m_methodTable.end(); ++it) {
    m_methodTable[it->first] = it->second;
  }
  m_attributes |= oces.m_attributes;
  m_initializedStatics |= oces.m_initializedStatics;
  m_initializedInstance |= oces.m_initializedInstance;
  m_doneSemanticCheck |= oces.m_doneSemanticCheck;
}
void ClassEvalState::fiberInitStatics(ClassEvalState &oces,
                                      FiberReferenceMap &refMap) {
  /**
   * Important to do a merge here, as m_statics may already have some states,
   * if this thread was processing another fiber job that hasn't been called
   * with FiberJob::getResults().
   */
  Array sv(oces.m_statics.fiberMarshal(refMap));
  for (ArrayIter iter(sv); iter; ++iter) {
    m_statics.set(iter.first(), iter.second());
  }
}
void ClassEvalState::fiberExit(ClassEvalState &oces,
                               FiberReferenceMap &refMap,
                               FiberAsyncFunc::Strategy default_strategy) {
  if (!m_class) {
    init(oces.m_class);
  } else if (m_class != oces.m_class) {
    // Class definition diverged
    raise_error("Different class of the same name (%s) defined in fiber",
                m_class->name().c_str());
  }
  if (default_strategy != FiberAsyncFunc::GlobalStateIgnore) {
    m_initializedStatics |= oces.m_initializedStatics;
  }

  if (oces.m_constructor) {
    m_constructor = oces.m_constructor;
  }
  for (MethodTable::const_iterator it = oces.m_methodTable.begin();
       it != oces.m_methodTable.end(); ++it) {
    m_methodTable[it->first] = it->second;
  }
  ASSERT(m_attributes == oces.m_attributes);
  m_initializedInstance |= oces.m_initializedInstance;
  m_doneSemanticCheck |= oces.m_doneSemanticCheck;
}
void ClassEvalState::fiberExitStatics(ClassEvalState &oces,
                                      FiberReferenceMap &refMap,
                                      FiberAsyncFunc::Strategy
                                      default_strategy) {
  refMap.unmarshal(m_statics, oces.m_statics, default_strategy);
}


IMPLEMENT_THREAD_LOCAL(RequestEvalState, s_res);
void RequestEvalState::Reset() {
  s_res->reset();
}

void RequestEvalState::reset() {
  m_functionStatics.clear();
  m_methodStatics.clear();

  m_classes.clear();
  m_functions.clear();
  m_constants = Array();
  m_constantInfos.clear();
  m_methodInfos.clear();
  m_classInfos.clear();
  m_ids = 0;
  m_argStack.clear();

  for (map<string, PhpFile*>::const_iterator it =
         m_evaledFiles.begin(); it != m_evaledFiles.end(); ++it) {
    if (it->second->decRef() == 0) {
      FileRepository::onZeroRef(it->second);
    }
  }
  m_codeContainers.clear();
  m_evaledFiles.clear();
  m_includes = Array();
}

void RequestEvalState::DestructObjects() {
  s_res->destructObjects();
}

void RequestEvalState::destructObject(EvalObjectData *eo) {
  m_livingObjects.erase(eo);
  int access = 0;
  const MethodStatement *des = eo->getMethodStatement("__destruct", access);
  if (des) {
    try {
      if (RuntimeOption::EnableObjDestructCall) {
          des->invokeInstance(Object(eo), Array());
      } else {
        raise_notice("Object of class %s is swept, "
                     "and therefore its destructor is not called",
                     eo->o_getClassName().c_str());
      }
    } catch (...) {
      handle_destructor_exception();
    }
  }
  eo->setInDtor();
}

void RequestEvalState::destructObjects() {
  // destruct the remaining objects
  while (!m_livingObjects.empty()) {
    EvalObjectData *eo = *m_livingObjects.begin();
    destructObject(eo);
  }
}

void RequestEvalState::addCodeContainer(SmartPtr<CodeContainer> &cc) {
  RequestEvalState *self = s_res.get();
  self->m_codeContainers.push_back(cc);
}

ClassEvalState &RequestEvalState::declareClass(const ClassStatement *cls) {
  RequestEvalState *self = s_res.get();
  String name = cls->name();
  if (self->m_classes.find(name) != self->m_classes.end() ||
      ClassInfo::FindClassInterfaceOrTrait(name)) {
    raise_error("Cannot redeclare class %s", name.c_str());
  }
  ClassEvalState &ce = self->m_classes[name];
  ce.init(cls);
  return ce;
}
void RequestEvalState::declareFunction(const FunctionStatement *fn) {
  RequestEvalState *self = s_res.get();
  String name = fn->name();
  if (fn->invalidOverride()) {
    if (fn->ignoredOverride()) return;
  } else if (get_renamed_function(name) == name) {
    std::pair<StringIMap<const FunctionStatement*>::iterator,
      bool> p = self->m_functions.insert(make_pair(name, fn));
    if (p.second) {
      UserFunctionIdTable::DeclareUserFunction(fn);
    }
    if (p.second ||
        (p.first->second == fn &&
         ParserBase::IsClosureOrContinuationName(name.c_str()))) return;
  }

  raise_error("Cannot redeclare %s()", name.c_str());
}

bool RequestEvalState::declareConstant(CStrRef name, CVarRef val) {
  RequestEvalState *self = s_res.get();
  if (self->m_constants.exists(name)) return false;
  self->m_constants.set(name, val);
  SmartPtr<EvalConstantInfo> &ci = self->m_constantInfos[name];
  ci = new EvalConstantInfo();
  // Only need to set value really.
  ci->valueLen = 0;
  ci->valueText = NULL;
  ci->setValue(val);
  return true;
}

ClassInfoEvaled::~ClassInfoEvaled() {
  for (MethodVec::iterator it = m_methodsVec.begin(); it != m_methodsVec.end();
       ++it) {
    MethodInfo *mi = *it;
    for (std::vector<const ParameterInfo*>::iterator mit =
           mi->parameters.begin();
         mit != mi->parameters.end(); ++mit) {
      free((void*)(*mit)->value);
    }
    delete mi;
  }
  for (PropertyMap::iterator it = m_properties.begin();
       it != m_properties.end(); ++it) {
    delete it->second;
  }
  for (ConstantMap::iterator it = m_constants.begin(); it != m_constants.end();
       ++it) {
    delete [] it->second->valueText;
    delete it->second;
  }
}

const ClassStatement *RequestEvalState::findClass(CStrRef name,
                                                  bool autoload /* = false */)
{
  RequestEvalState *self = s_res.get();
  StringIMap<ClassEvalState>::const_iterator it = self->m_classes.find(name);
  if (it != self->m_classes.end()) {
    return it->second.getClass();
  }
  if (autoload &&
      !ClassInfo::HasClassInterfaceOrTrait(name) &&
      eval_try_autoload(name)) {
    return findClass(name, false);
  }
  return NULL;
}

ClassEvalState *RequestEvalState::findClassState(CStrRef name,
                                                 bool autoload /* = false */) {
  RequestEvalState *self = s_res.get();
  StringIMap<ClassEvalState>::iterator it = self->m_classes.find(name);
  if (it != self->m_classes.end()) {
    return &it->second;
  }
  if (autoload && !ClassInfo::HasClassInterfaceOrTrait(name) &&
      eval_try_autoload(name)) {
    return findClassState(name, false);
  }
  return NULL;
}

const MethodStatement *RequestEvalState::findMethod(CStrRef cname,
                                                    CStrRef name,
                                                    ClassEvalState *&ce,
                                                    bool autoload /* = false */)
{
  ce = RequestEvalState::findClassState(cname, autoload);
  if (ce) {
    int access;
    return ce->getMethod(name, access);
  }
  return NULL;
}

const Function *RequestEvalState::findFunction(CStrRef name) {
  const Function *f = findUserFunction(name);
  if (f) return f;
  return evalOverrides.findFunction(name);
}
const FunctionStatement *RequestEvalState::findUserFunction(CStrRef name) {
  RequestEvalState *self = s_res.get();
  StringIMap<const FunctionStatement*>::const_iterator it =
    self->m_functions.find(name);
  if (it != self->m_functions.end()) {
    return it->second;
  }
  return NULL;
}

bool RequestEvalState::findConstant(CStrRef name, Variant &ret) {
  RequestEvalState *self = s_res.get();
  if (self->m_constants.exists(name)) {
    ret = self->m_constants.rvalAt(name);
    return true;
  }
  return false;
}

Variant RequestEvalState::findUserConstant(
  CStrRef name, bool error /* = true */) {
  RequestEvalState *self = s_res.get();
  if (self->m_constants.exists(name)) {
    return self->m_constants.rvalAt(name);
  }
  const char *s = name.c_str();
  if (error) raise_notice("Use of undefined constant %s - assumed '%s'", s, s);
  return name;
}

bool RequestEvalState::includeFile(Variant &res, CStrRef path, bool once,
                                   LVariableTable* variables,
                                   const char *currentDir) {
  RequestEvalState *self = s_res.get();

  // canonicalize the path and also check whether it is in an allowed
  // directory.
  String translated = File::TranslatePath(path, false, true);

  string spath(translated.data());
  struct stat s;
  if (!FileRepository::findFile(spath, &s)) return false;

  map<string, PhpFile*>::const_iterator it = self->m_evaledFiles.find(spath);
  Eval::PhpFile *efile = NULL;
  if (it != self->m_evaledFiles.end()) {
    if (once) {
      res = true;
      return true;
    }
    efile = it->second;
  } else {
    char rpath[PATH_MAX];
    bool alreadyResolved = !RuntimeOption::CheckSymLink && (spath[0] == '/');
    string checkoutPath(spath);
    if (alreadyResolved || realpath(spath.c_str(), rpath)) {
      it = self->m_evaledFiles.find(alreadyResolved ? spath.c_str() : rpath);
      if (it != self->m_evaledFiles.end()) {
        self->m_evaledFiles[spath] = efile = it->second;
        efile->incRef();
        if (once) {
          res = true;
          return true;
        }
      }
      if (alreadyResolved) {
        rpath[0] = 0;
      } else {
        checkoutPath = rpath;
      }
    } else {
      rpath[0] = 0;
    }
    if (!efile) {
      efile = FileRepository::checkoutFile(checkoutPath, s);
      if (efile) {
        self->m_evaledFiles[spath] = efile;
        if (rpath[0]) {
          self->m_evaledFiles[rpath] = efile;
          efile->incRef();
        }
      }
    }
  }
  if (efile) {
    res = efile->eval(variables);
    return true;
  }
  return false;
}

LVariableTable &RequestEvalState::
getFunctionStatics(const FunctionStatement* func) {
  RequestEvalState *self = s_res.get();
  return self->m_functionStatics[func];
}

LVariableTable &RequestEvalState::
getMethodStatics(const MethodStatement* func, CStrRef cls) {
  String c(empty_string);
  RequestEvalState *self = s_res.get();
  if (!(func->getModifiers() & ClassStatement::Private)) c = cls;
  return self->m_methodStatics[func][c];
}

LVariableTable *RequestEvalState::getClassStatics(const ClassStatement* cls) {
  RequestEvalState *self = s_res.get();
  StringIMap<ClassEvalState>::iterator it = self->m_classes.find(cls->name());
  if (it == self->m_classes.end()) return NULL;
  it->second.initializeStatics();
  return &it->second.getStatics();
}

string RequestEvalState::unique() {
  RequestEvalState *self = s_res.get();
  int64 id = self->m_ids++;
  stringstream ss;
  // Unique id contains self so that it will be unique even among fibers
  ss << (uint64)self << "_" << id;
  return ss.str();
}

Array RequestEvalState::getUserFunctionsInfo() {
  RequestEvalState *self = s_res.get();
  Array ret;
  for (StringIMap<const FunctionStatement*>::const_iterator it =
         self->m_functions.begin(); it != self->m_functions.end(); ++it) {
    ret.append(it->first);
  }
  return ret;
}
Array RequestEvalState::getClassesInfo() {
  RequestEvalState *self = s_res.get();
  Array ret;
  for (StringIMap<ClassEvalState>::const_iterator it = self->m_classes.begin();
       it != self->m_classes.end(); ++it) {
    if (it->second.getClass()->getModifiers() &
        (ClassStatement::Interface | ClassStatement::Trait))
      continue;
    ret.append(it->first);
  }
  return ret;
}
Array RequestEvalState::getInterfacesInfo() {
  RequestEvalState *self = s_res.get();
  Array ret;
  for (StringIMap<ClassEvalState>::const_iterator it = self->m_classes.begin();
       it != self->m_classes.end(); ++it) {
    if (it->second.getClass()->getModifiers() & ClassStatement::Interface) {
      ret.append(it->first);
    }
  }
  return ret;
}

Array RequestEvalState::getTraitsInfo() {
  RequestEvalState *self = s_res.get();
  Array ret;
  for (StringIMap<ClassEvalState>::const_iterator it = self->m_classes.begin();
       it != self->m_classes.end(); ++it) {
    if (it->second.getClass()->getModifiers() & ClassStatement::Trait) {
      ret.append(it->first);
    }
  }
  return ret;
}

Array RequestEvalState::getConstants() {
  RequestEvalState *self = s_res.get();
  return self->m_constants;
}

const ClassInfo::MethodInfo *RequestEvalState::findFunctionInfo(CStrRef name) {
  RequestEvalState *self = s_res.get();
  StringIMap<SmartPtr<EvalMethodInfo> >::iterator it =
    self->m_methodInfos.find(name);
  if (it == self->m_methodInfos.end()) {
    const FunctionStatement *fs = findUserFunction(name);
    if (fs) {
      SmartPtr<EvalMethodInfo> &m = self->m_methodInfos[name];
      m = new EvalMethodInfo();
      fs->getInfo(*m.get());
      return m.get();
    }
    return NULL;
  } else {
    return it->second.get();
  }
}

const ClassInfo *RequestEvalState::findClassLikeInfo(CStrRef name) {
  RequestEvalState *self = s_res.get();
  if (self->m_classInfos.empty() && self->m_classes.empty()) {
    // short cut for the compiled version
    return NULL;
  }
  StringMap<SmartPtr<ClassInfoEvaled> >::const_iterator it =
    self->m_classInfos.find(name);
  if (it == self->m_classInfos.end()) {
    const ClassStatement *cls = findClass(name);
    if (cls) {
      SmartPtr<ClassInfoEvaled> &cl = self->m_classInfos[name];
      cl = new ClassInfoEvaled();
      cls->getInfo(*cl.get());
      return cl.get();
    }
    return NULL;
  } else {
    return it->second.get();
  }
}

const ClassInfo::ConstantInfo *RequestEvalState::
findConstantInfo(CStrRef name) {
  RequestEvalState *self = s_res.get();
  StringMap<SmartPtr<EvalConstantInfo> >::const_iterator it =
    self->m_constantInfos.find(name);
  if (it != self->m_constantInfos.end()) {
    return it->second.get();
  }
  return NULL;
}

void RequestEvalState::registerObject(EvalObjectData *obj) {
  RequestEvalState *self = s_res.get();
  self->m_livingObjects.insert(obj);
}
void RequestEvalState::deregisterObject(EvalObjectData *obj) {
  RequestEvalState *self = s_res.get();
  self->m_livingObjects.erase(obj);
}

void RequestEvalState::GetMethodStaticVariables(Array &arr) {
  RequestEvalState *self = s_res.get();
  String prefix("sv_");
  for (map<const FunctionStatement*, LVariableTable>::iterator it =
      self->m_functionStatics.begin(); it != self->m_functionStatics.end();
      ++it) {
    String fprefix = prefix;
    fprefix += it->first->name();
    fprefix += "$$";
    Array vars(it->second.getDefinedVars());
    for (ArrayIter vit(vars); !vit.end(); vit.next()) {
      String key(fprefix);
      key += vit.first();
      arr.set(key, vit.second());
    }
  }

  for (MethodStatics::iterator it = self->m_methodStatics.begin();
      it != self->m_methodStatics.end(); ++it) {
    String mprefix(prefix);
    mprefix += it->first->getClass()->name();
    mprefix += "$$";
    mprefix += it->first->name();
    mprefix += "$$";
    Variant val;
    if (it->second.size() > 1) {
      for (StringIMap<LVariableTable>::iterator cit =
          it->second.begin(); cit != it->second.end(); ++cit) {
        Array vars(cit->second.getDefinedVars());
        for (ArrayIter vit(vars); !vit.end(); vit.next()) {
          arr.lvalAt(mprefix + vit.first()).set(cit->first, vit.second());
        }
      }
    } else {
      Array vars(it->second.begin()->second.getDefinedVars());
      for (ArrayIter vit(vars); !vit.end(); vit.next()) {
        arr.set(mprefix + vit.first(), vit.second());
      }
    }
  }
}

void RequestEvalState::GetClassStaticVariables(Array &arr) {
  RequestEvalState *self = s_res.get();
  String prefix("s_");
  for (StringIMap<ClassEvalState>::iterator it = self->m_classes.begin();
       it != self->m_classes.end(); ++it) {
    String cprefix(prefix);
    cprefix += it->first;
    cprefix += "$$";
    Array vars(it->second.getStatics().getDefinedVars());
    for (ArrayIter vit(vars); !vit.end(); vit.next()) {
      arr.set(cprefix + vit.first(), vit.second());
    }
  }
}

void RequestEvalState::GetDynamicConstants(Array &arr) {
  RequestEvalState *self = s_res.get();
  String prefix("k_");
  for (ArrayIter vit(self->m_constants); !vit.end(); vit.next()) {
    arr.set(prefix + vit.first(), vit.second());
  }
}

RequestEvalState *RequestEvalState::Get() {
  return s_res.get();
}

void RequestEvalState::fiberInit(RequestEvalState *res,
                                 FiberReferenceMap &refMap) {
  // Files
  for (map<std::string, PhpFile*>::iterator it = res->m_evaledFiles.begin();
      it != res->m_evaledFiles.end(); ++it) {
    m_evaledFiles[it->first] = it->second;
    it->second->incRef();
  }
  // Code containers
  for (list<SmartPtr<CodeContainer> >::iterator it =
      res->m_codeContainers.begin(); it != res->m_codeContainers.end();
      ++it) {
    addCodeContainer(*it);
  }
  // Functions
  for (StringIMap<const FunctionStatement*>::iterator it =
      res->m_functions.begin(); it != res->m_functions.end(); ++it) {
    m_functions[it->first] = it->second;
  }
  // Constants
  Array constants = res->m_constants.fiberMarshal(refMap);
  for (ArrayIter it(constants); it; ++it) {
    m_constants.set(it.first(), it.second());
  }
  Array includes = res->m_includes.fiberMarshal(refMap);
  for (ArrayIter it(includes); it; ++it) {
    m_includes.set(it.first(), it.second());
  }
  // Constant Info
  for (StringMap<SmartPtr<EvalConstantInfo> >::iterator it =
      res->m_constantInfos.begin(); it != res->m_constantInfos.end(); ++it) {
    m_constantInfos[it->first] = it->second;
  }
  // Method Info
  for (StringIMap<SmartPtr<EvalMethodInfo> >::iterator it =
       res->m_methodInfos.begin(); it != res->m_methodInfos.end(); ++it) {
    m_methodInfos[it->first] = it->second;
  }
  // Class Infos
  for (StringMap<SmartPtr<ClassInfoEvaled> >::iterator it =
      res->m_classInfos.begin(); it != res->m_classInfos.end(); ++it) {
    m_classInfos[it->first] = it->second;
  }
  // Classes
  for (StringIMap<ClassEvalState>::iterator it = res->m_classes.begin();
       it != res->m_classes.end(); ++it) {
    ClassEvalState &ces = m_classes[it->first];
    ClassEvalState &oces = it->second;
    ces.fiberInit(oces, refMap);
  }

  // Class Statics
  for (StringIMap<ClassEvalState>::iterator it =
       res->m_classes.begin(); it != res->m_classes.end(); ++it) {
    ClassEvalState &ces = m_classes[it->first];
    ClassEvalState &oces = it->second;
    ces.fiberInitStatics(oces, refMap);
  }
  // Function Statics
  for (map<const FunctionStatement*, LVariableTable>::const_iterator it =
      res->m_functionStatics.begin(); it != res->m_functionStatics.end();
      ++it) {
    m_functionStatics[it->first].
      Array::operator=(it->second.fiberMarshal(refMap));
  }
  // Method statics
  for (MethodStatics::const_iterator it = res->m_methodStatics.begin();
      it != res->m_methodStatics.end(); ++it) {
    for (StringIMap<LVariableTable>::const_iterator
         it2 = it->second.begin(); it2 != it->second.end(); ++it2) {
      m_methodStatics[it->first][it2->first].
        Array::operator=(it2->second.fiberMarshal(refMap));
    }
  }
}
void RequestEvalState::fiberExit(RequestEvalState *res,
                                 FiberReferenceMap &refMap,
                                 FiberAsyncFunc::Strategy default_strategy) {
  // Files
  for (map<std::string, PhpFile*>::iterator it = res->m_evaledFiles.begin();
      it != res->m_evaledFiles.end(); ++it) {
    if (m_evaledFiles.find(it->first) == m_evaledFiles.end()) {
      m_evaledFiles[it->first] = it->second;
      it->second->incRef();
    }
  }
  // Code containers
  for (list<SmartPtr<CodeContainer> >::iterator it =
      res->m_codeContainers.begin(); it != res->m_codeContainers.end();
      ++it) {
    // Could be re-adding it but that's ok
    addCodeContainer(*it);
  }
  // Functions
  for (StringIMap<const FunctionStatement*>::iterator it =
      res->m_functions.begin(); it != res->m_functions.end(); ++it) {
    StringIMap<const FunctionStatement*>::iterator fit =
      m_functions.find(it->first);
    if (fit == m_functions.end()) {
      m_functions[it->first] = it->second;
    } else if (fit->second != it->second) {
      raise_error("Different function of the same name (%s) defined in fiber",
                  fit->first.c_str());
    }
  }
  // Constants
  refMap.unmarshal(m_constants, res->m_constants, default_strategy);
  refMap.unmarshal(m_includes, res->m_includes, default_strategy);
  // Constant Info
  for (StringMap<SmartPtr<EvalConstantInfo> >::iterator it =
      res->m_constantInfos.begin(); it != res->m_constantInfos.end(); ++it) {
    m_constantInfos[it->first] = it->second;
  }
  // Method Info
  for (StringIMap<SmartPtr<EvalMethodInfo> >::iterator it =
       res->m_methodInfos.begin(); it != res->m_methodInfos.end(); ++it) {
    m_methodInfos[it->first] = it->second;
  }
  // Class Infos
  for (StringMap<SmartPtr<ClassInfoEvaled> >::iterator it =
      res->m_classInfos.begin(); it != res->m_classInfos.end(); ++it) {
    m_classInfos[it->first] = it->second;
  }
  // Classes
  for (StringIMap<ClassEvalState>::iterator it = res->m_classes.begin();
       it != res->m_classes.end(); ++it) {
    ClassEvalState &ces = m_classes[it->first];
    ClassEvalState &oces = it->second;
    ces.fiberExit(oces, refMap, default_strategy);
  }

  // Class Statics
  for (StringIMap<ClassEvalState>::iterator it = res->m_classes.begin();
       it != res->m_classes.end(); ++it) {
    ClassEvalState &ces = m_classes[it->first];
    ClassEvalState &oces = it->second;
    ces.fiberExitStatics(oces, refMap, default_strategy);
  }
  // Function Statics
  for (map<const FunctionStatement*, LVariableTable>::const_iterator it =
      res->m_functionStatics.begin(); it != res->m_functionStatics.end();
      ++it) {
    refMap.unmarshal((Array&)m_functionStatics[it->first], (Array&)it->second,
                     default_strategy);
  }
  // Method statics
  for (MethodStatics::const_iterator it = res->m_methodStatics.begin();
      it != res->m_methodStatics.end(); ++it) {
    for (StringIMap<LVariableTable>::const_iterator
         it2 = it->second.begin(); it2 != it->second.end(); ++it2) {
      refMap.unmarshal((Array&)m_methodStatics[it->first][it2->first],
                       (Array&)it2->second, default_strategy);
    }
  }
}

class EvalClassInfoHook : public ClassInfoHook {
public:
  EvalClassInfoHook() {
    if (HPHP::has_eval_support) ClassInfo::SetHook(this);
  }

  virtual Array getUserFunctions() const {
    return RequestEvalState::getUserFunctionsInfo();
  }
  virtual Array getClasses() const {
    return RequestEvalState::getClassesInfo();
  }
  virtual Array getInterfaces() const {
    return RequestEvalState::getInterfacesInfo();
  }
  virtual Array getTraits() const {
    return RequestEvalState::getTraitsInfo();
  }
  virtual Array getConstants() const {
    return RequestEvalState::getConstants();
  }
  virtual const ClassInfo::MethodInfo *findFunction(CStrRef name) const {
    return RequestEvalState::findFunctionInfo(name);
  }
  virtual const ClassInfo *findClassLike(CStrRef name) const {
    return RequestEvalState::findClassLikeInfo(name);
  }
  virtual const ClassInfo::ConstantInfo *findConstant(CStrRef name) const {
    return RequestEvalState::findConstantInfo(name);
  }
};
static EvalClassInfoHook eval_class_info_hook;

class EvalSourceInfoHook : public SourceInfoHook {
public:
  EvalSourceInfoHook() {
    if (HPHP::has_eval_support) SourceInfo::SetHook(this);
  }
  virtual const char *getClassDeclaringFile(CStrRef name,
                                            int *line = NULL) {
    const ClassStatement *f = RequestEvalState::findClass(name);
    if (f) {
      if (line) *line = f->loc()->line1;
      return f->loc()->file;
    } else {
      return NULL;
    }
  }
  virtual const char *getFunctionDeclaringFile(CStrRef name,
                                               int *line = NULL) {
    const FunctionStatement *f = RequestEvalState::findUserFunction(name);
    if (f) {
      if (line) *line = f->loc()->line1;
      return f->loc()->file;
    } else {
      return NULL;
    }
  }
};

static EvalSourceInfoHook eval_source_info_hook;

void RequestEvalState::info() {
  RequestEvalState *self = s_res.get();
  cerr << "Eval State Status" << endl;
  cerr << "-----------------" << endl;
  cerr << "Classes:" << endl;
  for (StringIMap<ClassEvalState>::const_iterator it = self->m_classes.begin();
       it != self->m_classes.end(); ++it) {
    cerr << " " << it->first << " " << it->second.getClass()->name() << endl;
  }
  cerr << "Functions:" << endl;
    for (StringIMap<const FunctionStatement*>::const_iterator it =
         self->m_functions.begin(); it != self->m_functions.end(); ++it) {
    cerr << " " << it->second->name() << endl;
  }
}

VariantStack &RequestEvalState::argStack() {
  RequestEvalState *self = s_res.get();
  return self->m_argStack;
}


VariantStack &RequestEvalState::bytecodeStack() {
  RequestEvalState *self = s_res.get();
  return self->m_bytecodeStack;
}


///////////////////////////////////////////////////////////////////////////////
}
}

