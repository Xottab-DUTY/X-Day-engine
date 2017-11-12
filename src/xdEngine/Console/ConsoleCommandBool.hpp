#pragma once

#include <string>

#include "ConsoleCommand.hpp"

namespace XDay
{
namespace Command
{
class XDENGINE_API Bool : public command
{
public:
    Bool(std::string _name, std::string _description, bool& _value, bool _save_allowed = true, bool _enabled = true);

    void Execute() override;
    void Execute(const std::string& args) override;
    void Execute(bool _value) const { value = _value; }

    std::string Info() const override { return "boolean value"; }
    std::string Syntax() const override { return "on/off, true/false, 1/0"; }
    std::string Status() const override { return value ? "on" : "off"; }

    bool GetValue() const { return value; }

protected:
    bool& value;
};
} // namespace Command
} // namespace XDay
