#include "pch.hpp"

#include "ConsoleCommand.hpp"

namespace XDay
{
namespace Command
{
command::command(std::string _name, std::string _description, bool _enabled, bool _lower_case_args, bool _empty_args_allowed, bool _save_allowed)
    : name(_name), description(_description), enabled(_enabled), lower_case_args(_lower_case_args),
      empty_args_allowed(_empty_args_allowed), save_allowed(_save_allowed) {}

command::~command() {}

void command::InvalidSyntax(const std::string& args) const
{
    Log::Warning("Invalid syntax in call [{} {}]", name, args);
    Log::Warning("Valid arguments: {}", Syntax());
}
} // namespace Command
} // namespace XDay
