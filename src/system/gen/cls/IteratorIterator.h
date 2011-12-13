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
// @generated by HipHop Compiler

#ifndef __GENERATED_cls_IteratorIterator_h0fa235b1__
#define __GENERATED_cls_IteratorIterator_h0fa235b1__

#include <runtime/base/hphp_system.h>
#include <system/gen/sys/literal_strings_remap.h>
#include <system/gen/sys/scalar_arrays_remap.h>
#include <cls/OuterIterator.h>

namespace HPHP {
///////////////////////////////////////////////////////////////////////////////

/* SRC: classes/iterator.php line 657 */
FORWARD_DECLARE_CLASS(IteratorIterator);
extern const ObjectStaticCallbacks cw_IteratorIterator;
class c_IteratorIterator : public ExtObjectData {
  public:

  // Properties
  Variant m_iterator;

  // Destructor
  ~c_IteratorIterator() NEVER_INLINE {}
  // Class Map
  DECLARE_CLASS_NO_SWEEP(IteratorIterator, IteratorIterator, ObjectData)
  static const ClassPropTable os_prop_table;
  c_IteratorIterator(const ObjectStaticCallbacks *cb = &cw_IteratorIterator) : ExtObjectData(cb), m_iterator(Variant::nullInit) {
    setAttribute(HasCall);
  }
  Variant doCall(Variant v_name, Variant v_arguments, bool fatal);
  public: void t___construct(Variant v_iterator);
  public: c_IteratorIterator *create(CVarRef v_iterator);
  public: Variant t_getinneriterator();
  public: Variant t_valid();
  public: Variant t_key();
  public: Variant t_current();
  public: Variant t_next();
  public: Variant t_rewind();
  public: Variant t___call(Variant v_func, Variant v_params);
  DECLARE_METHOD_INVOKE_HELPERS(__construct);
  DECLARE_METHOD_INVOKE_HELPERS(getinneriterator);
  DECLARE_METHOD_INVOKE_HELPERS(valid);
  DECLARE_METHOD_INVOKE_HELPERS(key);
  DECLARE_METHOD_INVOKE_HELPERS(current);
  DECLARE_METHOD_INVOKE_HELPERS(next);
  DECLARE_METHOD_INVOKE_HELPERS(rewind);
  DECLARE_METHOD_INVOKE_HELPERS(__call);
};
ObjectData *coo_IteratorIterator() NEVER_INLINE;

///////////////////////////////////////////////////////////////////////////////
}

#endif // __GENERATED_cls_IteratorIterator_h0fa235b1__
