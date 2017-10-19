#pragma once
#ifndef ConsoleCoreCommands_hpp__
#define ConsoleCoreCommands_hpp__

#include "Common/import_export_macros.hpp"

#include "xdEngine.hpp"
#include "ConsoleCommand.hpp"
#include "ConsoleCommandBool.hpp"
#include "ConsoleCommandCall.hpp"
#include "ConsoleCommandValue.hpp"

namespace XDay
{
namespace Command
{
namespace Functions
{
void quit();
void help(const std::string& args);
void config_save(const std::string& args);
void config_load(const std::string& args);
void flush_log();
} // namespace Functions

constexpr const char* no_description = "no description";
constexpr const char* test_description = "test function";
constexpr const char* self_description = "Function name describes itself";

static Call QuitCC("quit", self_description, Functions::quit, true);
static Call ExitCC("exit", self_description, Functions::quit, true);
static Call HelpCC("help", "Get list of available commands or specific command help", Functions::help, true);
static Call FlushLogCC("flush", "Flush the log", Functions::flush_log, true);
static Call ConfigSaveCC("config_save", "Save the config", Functions::config_save, true);
static Call ConfigLoadCC("config_load", "Load the config", Functions::config_load, true);
static Bool FullscreenCC("fullscreen", "Switch fullscreen", *&Engine.windowMainFullscreen);

XDAY_API void RegisterConsoleCommands();
} // namespace Command
} // namespace XDay

#endif // ConsoleCoreCommands_hpp__
