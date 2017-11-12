#pragma once

#include <string>

#include "ConsoleCommand.hpp"

namespace XDay
{
namespace Command
{
class XDENGINE_API String : public command
{
public:
    String(std::string _name, std::string _description, std::string _value, size_t _size = 0, bool _save_allowed = true, bool _enabled = true);

    void Execute() override;
    void Execute(const std::string& args) override;

    std::string Info() const override { return "string value"; }
    std::string Syntax() const override;
    std::string Status() const override { return value; }

    std::string GetValue() const { return value; }

protected:
    std::string value;
    size_t size;
};
} // namespace Command
} // namespace XDay
