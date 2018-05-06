#pragma once

#include "xdEngine/xdEngine.hpp"

namespace XDay
{
namespace Command
{
namespace Functions
{
XDENGINE_API void help(const std::string& args);
XDENGINE_API void config_save(const std::string& args);
XDENGINE_API void config_load(const std::string& args);
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

XDENGINE_API void RegisterConsoleCommands();
} // namespace Command
} // namespace XDay
