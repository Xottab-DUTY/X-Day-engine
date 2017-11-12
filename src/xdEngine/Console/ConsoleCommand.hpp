#pragma once

namespace XDay
{
namespace Command
{
class XDENGINE_API command
{
protected:
    std::string name;
    std::string description;
    bool enabled;
    bool lower_case_args;
    bool empty_args_allowed;
    bool save_allowed;

public:
    command(std::string _name, std::string _description, bool _enabled, bool _lower_case_args, bool _empty_args_allowed, bool _save_allowed);
    virtual ~command();

    virtual void Execute() = 0;
    virtual void Execute(const std::string& args) = 0;

    std::string GetName() const { return name; }
    bool isEnabled() const { return enabled; }
    bool isLowerCaseArgs() const { return lower_case_args; }
    bool isEmptyArgsAllowed() const { return empty_args_allowed; }
    bool isSaveAllowed() const { return save_allowed; }

    virtual std::string Status() const { return "no value"; }
    virtual std::string Info() const { return "interface for console commands"; }
    virtual std::string Syntax() const { return "no arguments"; }
    virtual std::string Help() const { return description; }
    void InvalidSyntax(const std::string& args) const;

    virtual std::string Save() { return name + " " + Status(); }
};
} // namespace Command
} // namespace XDay
