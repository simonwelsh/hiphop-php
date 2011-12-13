<?php
/**
 * Automatically generated by running "php schema.php debugger".
 *
 * You may modify the file, but re-running schema.php against this file will
 * standardize the format without losing your schema changes. It does lose
 * any changes that are not part of schema. Use "note" field to comment on
 * schema itself, and "note" fields are not used in any code generation but
 * only staying within this file.
 */
///////////////////////////////////////////////////////////////////////////////
// Preamble: C++ code inserted at beginning of ext_{name}.h

DefinePreamble(<<<CPP
#include <runtime/eval/debugger/debugger_client.h>
#include <runtime/eval/debugger/debugger_proxy.h>
CPP
);

///////////////////////////////////////////////////////////////////////////////
// Constants
//
// array (
//   'name' => name of the constant
//   'type' => type of the constant
//   'note' => additional note about this constant's schema
// )


///////////////////////////////////////////////////////////////////////////////
// Functions
//
// array (
//   'name'   => name of the function
//   'desc'   => description of the function's purpose
//   'flags'  => attributes of the function, see base.php for possible values
//   'opt'    => optimization callback function's name for compiler
//   'note'   => additional note about this function's schema
//   'return' =>
//      array (
//        'type'  => return type, use Reference for ref return
//        'desc'  => description of the return value
//      )
//   'args'   => arguments
//      array (
//        'name'  => name of the argument
//        'type'  => type of the argument, use Reference for output parameter
//        'value' => default value of the argument
//        'desc'  => description of the argument
//      )
// )

DefineFunction(
  array(
    'name'   => "hphpd_install_user_command",
    'desc'   => "Expands HipHop debugger with a user extended command.",
    'flags'  =>  HasDocComment | HipHopSpecific,
    'return' => array(
      'type'   => Boolean,
      'desc'   => "TRUE if the command name was not used before, FALSE otherwise.",
    ),
    'args'   => array(
      array(
        'name'   => "cmd",
        'type'   => String,
        'desc'   => "The command name to register.",
      ),
      array(
        'name'   => "clsname",
        'type'   => String,
        'desc'   => "The debugger command class that has its implementation.",
      ),
    ),
    'taint_observer' => false,
  ));

DefineFunction(
  array(
    'name'   => "hphpd_get_user_commands",
    'desc'   => "Gets a list of user extended commands.",
    'flags'  =>  HasDocComment | HipHopSpecific,
    'return' => array(
      'type'   => StringMap,
      'desc'   => "A map of commands and their PHP classes.",
    ),
    'taint_observer' => false,
  ));

DefineFunction(
  array(
    'name'   => "hphpd_break",
    'desc'   => "Sets a hard breakpoint. When a debugger is running, this line of code will break into debugger, if condition is met. If there is no debugger that's attached, it will not do anything.",
    'flags'  =>  HasDocComment | HipHopSpecific,
    'return' => array(
      'type'   => null,
    ),
    'args'   => array(
      array(
        'name'   => "condition",
        'type'   => Boolean,
        'value'  => "true",
        'desc'   => "If true, break, otherwise, continue.",
      ),
    ),
    'taint_observer' => false,
  ));

DefineFunction(
  array(
    'name'   => "hphpd_get_client",
    'desc'   => "Gets an hphpd client with a string key. With the same key, it guarantees to get the same debugger client across multiple requests.",
    'flags'  =>  HasDocComment | HipHopSpecific,
    'return' => array(
      'type'   => Variant,
    ),
    'args'   => array(
      array(
        'name'   => "name",
        'type'   => String,
        'value'  => "null",
        'desc'   => "the name to identify the debugger client",
      ),
    ),
    'taint_observer' => false,
  ));


///////////////////////////////////////////////////////////////////////////////
// Classes
//
// BeginClass
// array (
//   'name'   => name of the class
//   'desc'   => description of the class's purpose
//   'flags'  => attributes of the class, see base.php for possible values
//   'note'   => additional note about this class's schema
//   'parent' => parent class name, if any
//   'ifaces' => array of interfaces tihs class implements
//   'bases'  => extra internal and special base classes this class requires
//   'footer' => extra C++ inserted at end of class declaration
// )
//
// DefineConstant(..)
// DefineConstant(..)
// ...
// DefineFunction(..)
// DefineFunction(..)
// ...
// DefineProperty
// DefineProperty
//
// array (
//   'name'  => name of the property
//   'type'  => type of the property
//   'flags' => attributes of the property
//   'desc'  => description of the property
//   'note'  => additional note about this property's schema
// )
//
// EndClass()

///////////////////////////////////////////////////////////////////////////////

BeginClass(
  array(
    'name'   => "DebuggerProxyCmdUser",
    'desc'   => "DebuggerProxy wrapper for CmdUser",
    'flags'  =>  HasDocComment | HipHopSpecific,
    'footer' => <<<EOT

 public:
  Eval::DebuggerProxy *m_proxy;
EOT
,
  ));

DefineFunction(
  array(
    'name'   => "__construct",
    'desc'   => "Constructor of DebuggerProxyCmdUser.",
    'flags'  =>  HasDocComment | HipHopSpecific,
    'return' => array(
      'type'   => null,
    ),
  ));

DefineFunction(
  array(
    'name'   => "isLocal",
    'desc'   => "Whether this proxy is running locally for debugging a local script, or running remotely on a server.",
    'flags'  =>  HasDocComment | HipHopSpecific,
    'return' => array(
      'type'   => Boolean,
      'desc'   => "TRUE if locally, FALSE if remote.",
    ),
  ));

DefineFunction(
  array(
    'name'   => "send",
    'desc'   => "Sends a command back to DebuggerClientCmdUser.",
    'flags'  =>  HasDocComment | HipHopSpecific,
    'return' => array(
      'type'   => Variant,
      'desc'   => "TRUE if successful, FALSE otherwise.",
    ),
    'args'   => array(
      array(
        'name'   => "cmd",
        'type'   => Object,
        'desc'   => "The command to send to client.",
      ),
    ),
  ));

DefineFunction(
  array(
    'name'   => "__destruct",
    'desc'   => "Destructor of DebuggerProxyCmdUser.",
    'flags'  =>  HasDocComment | HipHopSpecific,
    'return' => array(
      'type'   => Variant,
      'desc'   => "Always returns null.",
    ),
  ));

EndClass(
);

///////////////////////////////////////////////////////////////////////////////

BeginClass(
  array(
    'name'   => "DebuggerClientCmdUser",
    'desc'   => "DebuggerClient wrapper for CmdUser",
    'flags'  =>  HasDocComment | HipHopSpecific,
    'footer' => <<<EOT

 public:
  Eval::DebuggerClient *m_client;
EOT
,
  ));

DefineConstant(
  array(
    'name'   => "AUTO_COMPLETE_FILENAMES",
    'type'   => Int64,
  ));

DefineConstant(
  array(
    'name'   => "AUTO_COMPLETE_VARIABLES",
    'type'   => Int64,
  ));

DefineConstant(
  array(
    'name'   => "AUTO_COMPLETE_CONSTANTS",
    'type'   => Int64,
  ));

DefineConstant(
  array(
    'name'   => "AUTO_COMPLETE_CLASSES",
    'type'   => Int64,
  ));

DefineConstant(
  array(
    'name'   => "AUTO_COMPLETE_FUNCTIONS",
    'type'   => Int64,
  ));

DefineConstant(
  array(
    'name'   => "AUTO_COMPLETE_CLASS_METHODS",
    'type'   => Int64,
  ));

DefineConstant(
  array(
    'name'   => "AUTO_COMPLETE_CLASS_PROPERTIES",
    'type'   => Int64,
  ));

DefineConstant(
  array(
    'name'   => "AUTO_COMPLETE_CLASS_CONSTANTS",
    'type'   => Int64,
  ));

DefineConstant(
  array(
    'name'   => "AUTO_COMPLETE_KEYWORDS",
    'type'   => Int64,
  ));

DefineConstant(
  array(
    'name'   => "AUTO_COMPLETE_CODE",
    'type'   => Int64,
  ));

DefineFunction(
  array(
    'name'   => "__construct",
    'desc'   => "Constructor of DebuggerClientCmdUser.",
    'flags'  =>  HasDocComment | HipHopSpecific,
    'return' => array(
      'type'   => null,
    ),
  ));

DefineFunction(
  array(
    'name'   => "quit",
    'desc'   => "Quits the client.",
    'flags'  =>  HasDocComment | HipHopSpecific,
    'return' => array(
      'type'   => null,
    ),
  ));

DefineFunction(
  array(
    'name'   => "print",
    'desc'   => "Prints some text without any color.",
    'flags'  =>  HasDocComment | HipHopSpecific | VariableArguments,
    'return' => array(
      'type'   => null,
    ),
    'args'   => array(
      array(
        'name'   => "format",
        'type'   => String,
        'desc'   => "Format string in printf() style.",
      ),
    ),
  ));

DefineFunction(
  array(
    'name'   => "help",
    'desc'   => "Prints some text in help color.",
    'flags'  =>  HasDocComment | HipHopSpecific | VariableArguments,
    'return' => array(
      'type'   => null,
    ),
    'args'   => array(
      array(
        'name'   => "format",
        'type'   => String,
        'desc'   => "Format string in printf() style.",
      ),
    ),
  ));

DefineFunction(
  array(
    'name'   => "info",
    'desc'   => "Prints some text in information color.",
    'flags'  =>  HasDocComment | HipHopSpecific | VariableArguments,
    'return' => array(
      'type'   => null,
    ),
    'args'   => array(
      array(
        'name'   => "format",
        'type'   => String,
        'desc'   => "Format string in printf() style.",
      ),
    ),
  ));

DefineFunction(
  array(
    'name'   => "output",
    'desc'   => "Prints some text in script output color.",
    'flags'  =>  HasDocComment | HipHopSpecific | VariableArguments,
    'return' => array(
      'type'   => null,
    ),
    'args'   => array(
      array(
        'name'   => "format",
        'type'   => String,
        'desc'   => "Format string in printf() style.",
      ),
    ),
  ));

DefineFunction(
  array(
    'name'   => "error",
    'desc'   => "Prints some text in error color.",
    'flags'  =>  HasDocComment | HipHopSpecific | VariableArguments,
    'return' => array(
      'type'   => null,
    ),
    'args'   => array(
      array(
        'name'   => "format",
        'type'   => String,
        'desc'   => "Format string in printf() style.",
      ),
    ),
  ));

DefineFunction(
  array(
    'name'   => "code",
    'desc'   => "Pretty print PHP source code.",
    'flags'  =>  HasDocComment | HipHopSpecific,
    'return' => array(
      'type'   => null,
    ),
    'args'   => array(
      array(
        'name'   => "source",
        'type'   => String,
        'desc'   => "PHP source code to print.",
      ),
      array(
        'name'   => "highlight_line",
        'type'   => Int32,
        'value'  => "0",
        'desc'   => "Which line to focus or highlight.",
      ),
      array(
        'name'   => "start_line_no",
        'type'   => Int32,
        'value'  => "0",
        'desc'   => "Starting line number. 0 for no line no.",
      ),
      array(
        'name'   => "end_line_no",
        'type'   => Int32,
        'value'  => "0",
        'desc'   => "End line number. 0 for no end line no.",
      ),
    ),
  ));

DefineFunction(
  array(
    'name'   => "ask",
    'desc'   => "Ask end user a question.",
    'flags'  =>  HasDocComment | HipHopSpecific | VariableArguments,
    'return' => array(
      'type'   => Variant,
      'desc'   => "Single letter response from end user.",
    ),
    'args'   => array(
      array(
        'name'   => "format",
        'type'   => String,
        'desc'   => "Format string in printf() style.",
      ),
    ),
  ));

DefineFunction(
  array(
    'name'   => "wrap",
    'desc'   => "Wraps some text to fit screen width.",
    'flags'  =>  HasDocComment | HipHopSpecific,
    'return' => array(
      'type'   => String,
      'desc'   => "Formatted string.",
    ),
    'args'   => array(
      array(
        'name'   => "str",
        'type'   => String,
        'desc'   => "String to wrap.",
      ),
    ),
  ));

DefineFunction(
  array(
    'name'   => "helpTitle",
    'desc'   => "Displays a title for a help topic.",
    'flags'  =>  HasDocComment | HipHopSpecific,
    'return' => array(
      'type'   => null,
    ),
    'args'   => array(
      array(
        'name'   => "str",
        'type'   => String,
        'desc'   => "Title text.",
      ),
    ),
  ));

DefineFunction(
  array(
    'name'   => "helpCmds",
    'desc'   => "Displays a list of commands in help format. Each command has a name and a short description, and specify more commands in pairs. For example, \$client->helpCmds('cmd1', 'desc1', 'cmd2', 'desc2').",
    'flags'  =>  HasDocComment | HipHopSpecific | VariableArguments,
    'return' => array(
      'type'   => null,
    ),
    'args'   => array(
      array(
        'name'   => "cmd",
        'type'   => String,
        'desc'   => "Command name.",
      ),
      array(
        'name'   => "desc",
        'type'   => String,
        'desc'   => "Command description.",
      ),
    ),
  ));

DefineFunction(
  array(
    'name'   => "helpBody",
    'desc'   => "Displays help contents. A help body is a help section with one empty line before and one empty line after.",
    'flags'  =>  HasDocComment | HipHopSpecific,
    'return' => array(
      'type'   => null,
    ),
    'args'   => array(
      array(
        'name'   => "str",
        'type'   => String,
        'desc'   => "The help text.",
      ),
    ),
  ));

DefineFunction(
  array(
    'name'   => "helpSection",
    'desc'   => "Displays a section of help text.",
    'flags'  =>  HasDocComment | HipHopSpecific,
    'return' => array(
      'type'   => null,
    ),
    'args'   => array(
      array(
        'name'   => "str",
        'type'   => String,
        'desc'   => "One section of help text.",
      ),
    ),
  ));

DefineFunction(
  array(
    'name'   => "tutorial",
    'desc'   => "Tutorials are help texts displayed according to user's preference. In auto mode (vs. always on or always off modes), one tutorial text is only displayed just once to end user.",
    'flags'  =>  HasDocComment | HipHopSpecific,
    'return' => array(
      'type'   => null,
    ),
    'args'   => array(
      array(
        'name'   => "str",
        'type'   => String,
        'desc'   => "Help texts guiding end user for learning how to use debugger.",
      ),
    ),
  ));

DefineFunction(
  array(
    'name'   => "getCode",
    'desc'   => "PHP code snippet user just typed in manually.",
    'flags'  =>  HasDocComment | HipHopSpecific,
    'return' => array(
      'type'   => String,
      'desc'   => "The PHP source code.",
    ),
  ));

DefineFunction(
  array(
    'name'   => "getCommand",
    'desc'   => "Debugger command end user typed.",
    'flags'  =>  HasDocComment | HipHopSpecific,
    'return' => array(
      'type'   => String,
      'desc'   => "The command text.",
    ),
  ));

DefineFunction(
  array(
    'name'   => "arg",
    'desc'   => "Tests if an argument matches a pre-defined keyword. As long as it matches the keyword partially and case-insensitively, it is considered as a match. For example, \$client->arg(2, 'foo') will return TRUE if user inputs 'f' or 'fo' or 'Fo' for the 2nd argument.",
    'flags'  =>  HasDocComment | HipHopSpecific,
    'return' => array(
      'type'   => Boolean,
      'desc'   => "TRUE if matched. FALSE otherwise.",
    ),
    'args'   => array(
      array(
        'name'   => "index",
        'type'   => Int32,
        'desc'   => "Argument index.",
      ),
      array(
        'name'   => "str",
        'type'   => String,
        'desc'   => "The string to compare with.",
      ),
    ),
  ));

DefineFunction(
  array(
    'name'   => "argCount",
    'desc'   => "Count of total arguments.",
    'flags'  =>  HasDocComment | HipHopSpecific,
    'return' => array(
      'type'   => Int32,
      'desc'   => "The count, not including user command itself.",
    ),
  ));

DefineFunction(
  array(
    'name'   => "argValue",
    'desc'   => "Gets value of an argument.",
    'flags'  =>  HasDocComment | HipHopSpecific,
    'return' => array(
      'type'   => String,
      'desc'   => "String value of an argument.",
    ),
    'args'   => array(
      array(
        'name'   => "index",
        'type'   => Int32,
        'desc'   => "Argument index.",
      ),
    ),
  ));

DefineFunction(
  array(
    'name'   => "argRest",
    'desc'   => "Gets remaining arguments all together as a single string.",
    'flags'  =>  HasDocComment | HipHopSpecific,
    'return' => array(
      'type'   => String,
      'desc'   => "The string that has all argument at and after certain index.",
    ),
    'args'   => array(
      array(
        'name'   => "index",
        'type'   => Int32,
        'desc'   => "The starting index to include arguments.",
      ),
    ),
  ));

DefineFunction(
  array(
    'name'   => "args",
    'flags'  =>  HasDocComment | HipHopSpecific,
    'return' => array(
      'type'   => StringVec,
    ),
  ));

DefineFunction(
  array(
    'name'   => "send",
    'desc'   => "Sends a debugger command to debugger proxy.",
    'flags'  =>  HasDocComment | HipHopSpecific,
    'return' => array(
      'type'   => Variant,
      'desc'   => "TRUE if successful, FALSE otherwise.",
    ),
    'args'   => array(
      array(
        'name'   => "cmd",
        'type'   => Object,
        'desc'   => "The command to send.",
      ),
    ),
  ));

DefineFunction(
  array(
    'name'   => "xend",
    'desc'   => "Exchanges command with proxy: sends a command to debugger and expects and receives a command from debugger.",
    'flags'  =>  HasDocComment | HipHopSpecific,
    'return' => array(
      'type'   => Variant,
      'desc'   => "The received command, and it is always the same type as what it sends, so the same command class can handle processing at both sending and receiving sides.",
    ),
    'args'   => array(
      array(
        'name'   => "cmd",
        'type'   => Object,
        'desc'   => "The command to send.",
      ),
    ),
  ));

DefineFunction(
  array(
    'name'   => "getCurrentLocation",
    'desc'   => "Gets current source location.",
    'flags'  =>  HasDocComment | HipHopSpecific,
    'return' => array(
      'type'   => Variant,
      'desc'   => "An array in a format of array('file' => {source file name}, 'line' => {line number}, 'namespace' => {namespace code is in}, 'class' => {class code is in}, 'function' => {function code is in}, 'text' => {human readable description of current source location}).",
    ),
  ));

DefineFunction(
  array(
    'name'   => "getStackTrace",
    'desc'   => "Gets current stacktrace.",
    'flags'  =>  HasDocComment | HipHopSpecific,
    'return' => array(
      'type'   => Variant,
      'desc'   => "An array of stacktrace frames.",
    ),
  ));

DefineFunction(
  array(
    'name'   => "getFrame",
    'desc'   => "Returns current frame index.",
    'flags'  =>  HasDocComment | HipHopSpecific,
    'return' => array(
      'type'   => Int32,
      'desc'   => "An index indicating which frame end user has moved to for inspection.",
    ),
  ));

DefineFunction(
  array(
    'name'   => "printFrame",
    'desc'   => "Prints a stacktrace frame.",
    'flags'  =>  HasDocComment | HipHopSpecific,
    'return' => array(
      'type'   => null,
    ),
    'args'   => array(
      array(
        'name'   => "index",
        'type'   => Int32,
        'desc'   => "Which frame to print.",
      ),
    ),
  ));

DefineFunction(
  array(
    'name'   => "addCompletion",
    'desc'   => "Adds string(s) to auto-completion. This function is only effective inside DebuggerClientCmdUser::onAutoComplete().",
    'flags'  =>  HasDocComment | HipHopSpecific,
    'return' => array(
      'type'   => null,
    ),
    'args'   => array(
      array(
        'name'   => "list",
        'type'   => Variant,
        'desc'   => "A single string, an AUTO_COMPLETE_ constant or an array of strings.",
      ),
    ),
  ));

DefineFunction(
  array(
    'name'   => "__destruct",
    'desc'   => "Destructor of DebuggerClientCmdUser.",
    'flags'  =>  HasDocComment | HipHopSpecific,
    'return' => array(
      'type'   => Variant,
      'desc'   => "Always returns null.",
    ),
  ));

EndClass(
);

///////////////////////////////////////////////////////////////////////////////

BeginClass(
  array(
    'name'   => "DebuggerClient",
    'desc'   => "",
    'bases'  => array('Sweepable'),
    'flags'  =>  HasDocComment | HipHopSpecific | NoDefaultSweep,
    'footer' => <<<EOT

 public:
  Eval::DebuggerClient *m_client;
EOT
,
  ));

DefineConstant(
  array(
    'name'   => "STATE_INVALID",
    'type'   => Int64,
  ));

DefineConstant(
  array(
    'name'   => "STATE_UNINIT",
    'type'   => Int64,
  ));

DefineConstant(
  array(
    'name'   => "STATE_INITIALIZING",
    'type'   => Int64,
  ));

DefineConstant(
  array(
    'name'   => "STATE_READY_FOR_COMMAND",
    'type'   => Int64,
  ));

DefineConstant(
  array(
    'name'   => "STATE_BUSY",
    'type'   => Int64,
  ));

DefineFunction(
  array(
    'name'   => "__construct",
    'desc'   => "Constructor",
    'flags'  =>  HasDocComment | HipHopSpecific,
    'return' => array(
      'type'   => null,
    ),
  ));

DefineFunction(
  array(
    'name'   => "getState",
    'desc'   => "get current state of the debugger client",
    'flags'  => HasDocComment | HipHopSpecific,
    'return' => array(
      'type'   => Int64,
      'desc'   => "One of the constants",
    ),
  ));

DefineFunction(
  array(
    'name'   => "init",
    'desc'   => "initialize the debugger client",
    'flags'  => HasDocComment | HipHopSpecific,
    'return' => array(
      'type'   => Variant,
      'desc'   => "TBD",
    ),
    'args'   => array(
      array(
        'name'   => "options",
        'type'   => Variant,
        'desc'   => "array for passing options",
      ),
    ),
  ));

DefineFunction(
  array(
    'name'   => "processCmd",
    'desc'   => "issue a command to debugger client",
    'flags'  => HasDocComment | HipHopSpecific,
    'return' => array(
      'type'   => Variant,
      'desc'   => "TBD",
    ),
    'args'   => array(
      array(
        'name'   => "cmdName",
        'type'   => Variant,
        'desc'   => "name of the command to be executed",
      ),
      array(
        'name'   => "args",
        'type'   => Variant,
        'desc'   => "A vector array of strings to be used as arguments",
      ),
    ),
  ));

DefineFunction(
  array(
    'name'   => "interrupt",
    'desc'   => "make debugger client issue an CmdSignal as user doing Ctrl_C",
    'flags'  => HasDocComment | HipHopSpecific,
    'return' => array(
      'type'   => Variant,
      'desc'   => "TBD",
    ),
  ));

EndClass(
);
