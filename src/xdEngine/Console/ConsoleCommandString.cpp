#include "pch.hpp"

#include "ConsoleCommandString.hpp"
#include <fmt/format.h>

namespace XDay
{
namespace Command
{
String::String(std::string _name, std::string _description, std::string _value, size_t _size, bool _save_allowed, bool _enabled)
    : command(_name, _description, _enabled, false, false, _save_allowed), value(_value), size(_size) {}

void String::Execute() {}

void String::Execute(const std::string& args)
{
    if (size != 0 && args.length() > size)
    {
        InvalidSyntax(args);
        return;
    }
    value = args;
}

std::string String::Syntax() const
{
    return size == 0 ? "max size is not defined" : fmt::format("max size is {}", size);
}
} // namespace Command
} // namespace XDay
