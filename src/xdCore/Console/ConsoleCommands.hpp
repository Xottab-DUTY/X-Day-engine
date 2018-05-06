#pragma once

#include "ConsoleCommand.hpp"

namespace XDay::Console
{
namespace Calls
{
XDCORE_API void Crash();
XDCORE_API void Help();
XDCORE_API void Help(stringc&& args);
} // namespace Calls

constexpr cpcstr noDescription = "no description";
constexpr cpcstr testDescription = "test function";
constexpr cpcstr selfDescription = "Function name describes itself";

// Waiting for "inline" keyword...
extern XDCORE_API Command<Call> Terminate;
extern XDCORE_API Command<Call> Crash;
extern XDCORE_API Command<Call> FlushLog;
extern XDCORE_API Command<Call> Help;

class XDCORE_API Commands
{
    vector<ICommand*> commands;
    static Commands instance;

public:
    static const auto Get() { return instance.commands; }

    static void Register(ICommand* command) noexcept;
    static void Unregister(ICommand* command) noexcept;

    static bool Registered(stringc&& needed);
    static bool Registered(ICommand* needed);

    static ICommand* Get(stringc&& needed);

    template <typename Type>
    static Type GetValue(stringc&& command);

    static bool Execute(cpcstr raw);

    static bool Execute(stringc&& command);
    static bool Execute(stringc&& command, string&& args);

    static bool Execute(ICommand* command);
    static bool Execute(ICommand* command, string&& args);
};



} // namespace XDay::Console
