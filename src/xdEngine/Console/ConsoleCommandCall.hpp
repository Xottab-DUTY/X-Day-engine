#pragma once

#include <string>

#include "Common/import_export_macros.hpp"

#include "ConsoleCommand.hpp"

namespace XDay
{
namespace Command
{
class XDENGINE_API Call : public command
{
public:
    Call(std::string _name, std::string _description, void(*_function)(const std::string&), bool _AllowEmptyArgs = false, bool _enabled = true);
    Call(std::string _name, std::string _description, void(*_function)(), bool _enabled = true);

    void Execute(const std::string& args) override;
    void Execute() override;

    std::string Info() const override { return "function call"; }
protected:
    void(*function)(const std::string& args);
    void(*function_no_args)();

};
} // namespace Command
} // namespace XDay
