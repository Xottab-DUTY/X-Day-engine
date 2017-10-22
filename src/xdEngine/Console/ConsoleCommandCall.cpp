#include "pch.hpp"

#include "ConsoleCommandCall.hpp"

namespace XDay
{
namespace Command
{
Call::Call(std::string _name, std::string _description, void(*_function)(const std::string&), bool _empty_args_allowed, bool _enabled)
    : command(_name, _description, _enabled, true, _empty_args_allowed, false), function(_function), function_no_args(nullptr) {}

Call::Call(std::string _name, std::string _description, void(*_function)(), bool _enabled)
    : command(_name, _description, _enabled, true, true, false), function(nullptr), function_no_args(_function) {}

void Call::Execute(const std::string& args)
{
    if (function)
        function(args);
    else if (isEmptyArgsAllowed() && function_no_args)
        function_no_args();
    else Status();
}

void Call::Execute()
{
    if (function_no_args)
        function_no_args();
    else if (isEmptyArgsAllowed() && function)
        function("");
    else Status();
}
} // namespace Command
} // namespace XDay
