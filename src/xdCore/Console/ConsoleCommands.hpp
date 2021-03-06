#pragma once

#include "ConsoleCommand.hpp"

namespace XDay::Console
{
namespace Calls
{
XDCORE_API void Crash();
XDCORE_API void Help(stringc&& args);
XDCORE_API void CommandLine(stringc&& args);
} // namespace Calls

constexpr cpcstr noDescription = "no description";
constexpr cpcstr testDescription = "test function";
constexpr cpcstr selfDescription = "Function name describes itself";

// Waiting for "inline" keyword...
static Command<Call> Terminate("terminate", selfDescription, [] { std::terminate(); });
static Command<Call> Crash("crash", selfDescription, Calls::Crash);
static Command<Call> FlushLog("flush", selfDescription, [] { Log::Flush(); });
static Command<CallWithArgs> Help("help", "Get list of available commands or specific command help", Calls::Help, true);
static Command<CallWithArgs> Cmd("cmd", "Get list of available keys or specific key help", Calls::CommandLine, true);

class XDCORE_API Commands
{
    vector<ICommand*> commands;
    static Commands instance;

public:
    static const auto Get() { return instance.commands; }

    static void Initialize();
    static void Destroy();

    static void Register(ICommand* command) noexcept;
    static void Unregister(ICommand* command) noexcept;

    static bool Registered(stringc&& needed);
    static bool Registered(ICommand* needed);

    static ICommand* Get(stringc&& needed);

    template <typename type>
    static type GetValue(stringc&& command)
    {
        auto icmd = Get(std::move(command));

        auto cmd = static_cast<Command<type>*>(icmd);
        if (cmd)
            return cmd->Value();

        Log::Error("Console: failed to find value for [{}]", std::move(command));
        // XXX: Crash here?
        return type(0);
    }

    static bool Execute(cpcstr raw);

    static bool Execute(stringc&& command);
    static bool Execute(stringc&& command, string&& args);

    static bool Execute(ICommand* command);
    static bool Execute(ICommand* command, string&& args);
};
} // namespace XDay::Console
