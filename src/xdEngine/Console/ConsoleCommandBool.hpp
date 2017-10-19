#pragma once
#ifndef ConsoleCommandBool_hpp__
#define ConsoleCommandBool_hpp__

#include <string>

#include "Common/import_export_macros.hpp"

#include "ConsoleCommand.hpp"

namespace XDay
{
namespace Command
{
class XDAY_API Bool : public command
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

#endif // ConsoleCommandBool_h__