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

static Command<Call> Quit("quit", selfDescription, { Calls::Quit, nullptr});
static Command<Call> Exit("exit", selfDescription, { Calls::Quit, nullptr });
static Command<Call> ConfigSave("config_save", selfDescription, { [] { Calls::ConfigSave(""); }, Calls::ConfigSave });
static Command<Call> ConfigLoad("config_load", selfDescription, { [] { Calls::ConfigLoad(""); }, Calls::ConfigLoad });

static Command<bool> Fullscreen("fullscreen", "Switch fullscreen", Engine.windowMainFullscreen);

XDENGINE_API void RegisterEngineCommands();
} // XDay::Console
