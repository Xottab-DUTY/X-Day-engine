#pragma once

#include "xdCore/Console/ConsoleCommand.hpp"
#include "xdCore/Console/ConsoleCommands.hpp"
#include "xdEngine/xdEngine.hpp"

namespace XDay::Console
{
namespace Calls
{
XDENGINE_API void Quit();
XDENGINE_API void ConfigSave(stringc&& args);
XDENGINE_API void ConfigLoad(stringc&& args);
} // namespace Calls

static Command<Call> Quit("quit", selfDescription, Calls::Quit);
static Command<Call> Exit("exit", selfDescription, Calls::Quit);
static Command<CallWithArgs> ConfigSave("config_save", selfDescription, Calls::ConfigSave);
static Command<CallWithArgs> ConfigLoad("config_load", selfDescription, Calls::ConfigLoad);

static Command<bool> Fullscreen("fullscreen", "Switch fullscreen", Engine.windowMainFullscreen);

XDENGINE_API void RegisterEngineCommands();
} // XDay::Console
