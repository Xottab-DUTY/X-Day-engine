#pragma once

#include "xdCore/xdCore.hpp"
#include "ConsoleCommand.hpp"
#include "xdEngine.hpp"

/*
A bit of help:

CC_FunctionCall ("command_name_in_console", function_to_call, is_empty_args_allowed);

CC_Bool         ("command_name_in_console", variable_to_change);

CC_Toggle       ("command_name_in_console", variable_to_change);

CC_Integer      ("command_name_in_console", variable_to_change, minimum_value, maximum_value);

CC_Float        ("command_name_in_console", variable_to_change, minimum_value, maximum_value);

CC_Double       ("command_name_in_console", variable_to_change, minimum_value, maximum_value);

CC_String       ("command_name_in_console", variable_to_change, max_string_size);
*/

#pragma region Console commands
// Don't put here not ready testing functions, use CMD0-4 in RegisterConsoleCommands() for them

namespace cc_functions
{
    void CC_Exit(const std::string& args);
    void CC_Help(const std::string& args);

    void CC_ConfigLoad(const std::string& args);
    void CC_ConfigSave(const std::string& args);

    void CC_FlushLog(const std::string& args);
}

static XDay::CC_FunctionCall ExitCC("exit", cc_functions::CC_Exit, true);
static XDay::CC_FunctionCall QuitCC("quit", cc_functions::CC_Exit, true);
static XDay::CC_FunctionCall HelpCC("help", cc_functions::CC_Help, true);

static XDay::CC_FunctionCall FlushLogCC("flush", cc_functions::CC_FlushLog, true);

static XDay::CC_FunctionCall ConfigLoadCC("config_load", cc_functions::CC_ConfigLoad, true);
static XDay::CC_FunctionCall ConfigSaveCC("config_save", cc_functions::CC_ConfigSave, true);

static XDay::CC_Bool FullscreenCC("r_fullscreen", *&Engine.windowMainFullscreen);

void RegisterConsoleCommands();
#pragma endregion Console commands
