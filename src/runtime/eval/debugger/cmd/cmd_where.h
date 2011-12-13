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

#ifndef __HPHP_EVAL_DEBUGGER_CMD_WHERE_H__
#define __HPHP_EVAL_DEBUGGER_CMD_WHERE_H__

#include <runtime/eval/debugger/debugger_command.h>

namespace HPHP { namespace Eval {
///////////////////////////////////////////////////////////////////////////////

DECLARE_BOOST_TYPES(CmdWhere);
class CmdWhere : public DebuggerCommand {
public:
  CmdWhere() : DebuggerCommand(KindOfWhere) {}

  virtual bool help(DebuggerClient *client);

  virtual bool onClient(DebuggerClient *client);
  virtual void setClientOutput(DebuggerClient *client);
  virtual bool onServer(DebuggerProxy *proxy);
  virtual bool onServerVM(DebuggerProxy *proxy);

  virtual void sendImpl(DebuggerThriftBuffer &thrift);
  virtual void recvImpl(DebuggerThriftBuffer &thrift);

  Array fetchStackTrace(DebuggerClient *client);

private:
  Array m_stacktrace;
};

///////////////////////////////////////////////////////////////////////////////
}}

#endif // __HPHP_EVAL_DEBUGGER_CMD_WHERE_H__
