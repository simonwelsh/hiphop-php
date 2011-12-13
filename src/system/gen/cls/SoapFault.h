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

#ifndef __GENERATED_cls_SoapFault_h2cb1e2a5__
#define __GENERATED_cls_SoapFault_h2cb1e2a5__

#include <runtime/base/hphp_system.h>
#include <system/gen/sys/literal_strings_remap.h>
#include <system/gen/sys/scalar_arrays_remap.h>
#include <cls/Exception.h>

namespace HPHP {
///////////////////////////////////////////////////////////////////////////////

/* SRC: classes/soapfault.php line 3 */
FORWARD_DECLARE_CLASS(SoapFault);
extern const ObjectStaticCallbacks cw_SoapFault;
class c_SoapFault : public c_Exception {
  public:

  // Properties
  Variant m_faultcode;
  Variant m_faultcodens;
  Variant m_faultstring;
  Variant m_faultactor;
  Variant m_detail;
  Variant m__name;
  Variant m_headerfault;

  // Destructor
  ~c_SoapFault() NEVER_INLINE {}
  // Class Map
  DECLARE_CLASS_NO_SWEEP(SoapFault, SoapFault, Exception)
  static const ClassPropTable os_prop_table;
  c_SoapFault(const ObjectStaticCallbacks *cb = &cw_SoapFault) : c_Exception(cb), m_faultcode(Variant::nullInit), m_faultcodens(Variant::nullInit), m_faultstring(Variant::nullInit), m_faultactor(Variant::nullInit), m_detail(Variant::nullInit), m__name(Variant::nullInit), m_headerfault(Variant::nullInit) {}
  public: void t___construct(Variant v_code, Variant v_message, Variant v_actor = null, Variant v_detail = null, Variant v_name = null, Variant v_header = null);
  public: c_SoapFault *create(CVarRef v_code, CVarRef v_message, CVarRef v_actor = null_variant, CVarRef v_detail = null_variant, CVarRef v_name = null_variant, CVarRef v_header = null_variant);
  public: String t___tostring();
  DECLARE_METHOD_INVOKE_HELPERS(__construct);
  DECLARE_METHOD_INVOKE_HELPERS(__tostring);
};
ObjectData *coo_SoapFault() NEVER_INLINE;

///////////////////////////////////////////////////////////////////////////////
}

#endif // __GENERATED_cls_SoapFault_h2cb1e2a5__
