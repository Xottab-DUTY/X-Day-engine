#include "pch.hpp"

#include "ConsoleCommandBool.hpp"

namespace XDay
{
namespace Command
{
Bool::Bool(std::string _name, std::string _description, bool& _value, bool _save_allowed /*= true*/, bool _enabled /*= true*/)
    : command(_name, _description, _enabled, true, false, _save_allowed), value(_value) {}

void Bool::Execute() {}

void Bool::Execute(const std::string& args)
{
    bool v;
    if (args.compare("on") == 0 || args.compare("true") == 0 || args.compare("1") == 0)
    {
        v = true;
    }
    else if (args.compare("off") == 0 || args.compare("false") == 0 || args.compare("0") == 0)
    {
        v = false;
    }
    else
    {
        InvalidSyntax(args);
        return;
    }
    value = v;
}
} // namespace Command
} // namespace XDay
