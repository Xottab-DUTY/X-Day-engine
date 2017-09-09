#include <sstream>
#include <fstream>
#include <locale>

#include <GLFW/glfw3.h>

#include "Debug/Log.hpp"
#include "xdCore.hpp"
#include "xdEngine.hpp"
#include "ConsoleCommand.hpp"
#include "XMLResource.hpp"

using namespace XDay;

#pragma region ConsoleCommand Container
bool CC_Container::Execute(std::string cmd) const
{
    std::string cmd_str, cmd_val;

    std::istringstream buffer(cmd);
    buffer >> cmd_str;
    for (std::string i; buffer >> i;)
    {
        cmd_val += i + " ";
    }
    if (!cmd_val.empty())
        cmd_val.pop_back(); // remove the last " "

    ConsoleCommand* command(GetCommand(cmd_str));
    if (command)
    {
        if (command->isEnabled())
        {
            if (command->isLowerCaseArgs())
            {
                std::locale loc;
                for (auto&& elem : cmd_val)
                    elem = std::tolower(elem, loc);
            }
            if (cmd_val.empty())
            {
                if (command->isEmptyArgsAllowed())
                    command->Execute(cmd_val);
                else
                    Info("{} {}", command->GetName(), command->Status());
            }
            else
                command->Execute(cmd_val);
        }
        else
        {
            Info("Command is disabled: {}", cmd_str);
            return false;
        }
    }
    else
    {
        Info("Unknown command: {}", cmd_str);
        return false;
    }
    return true;
}

bool CC_Container::Execute(ConsoleCommand* cmd) const
{
    if (cmd)
    {
        if (cmd->isEnabled())
        {
            if (cmd->isEmptyArgsAllowed())
                cmd->Execute("");
            else
                Info("{} {}", cmd->GetName(), cmd->Status());
        }
        else
        {
            Info("Command is disabled: {}", cmd->GetName());
            return false;
        }
    }
    else
    {
        Warning("Unknown command.");
        return false;
    }
    return true;
}

bool CC_Container::Execute(ConsoleCommand* cmd, std::string args) const
{
    if (cmd)
        if (cmd->isEnabled())
        {
            if (cmd->isLowerCaseArgs())
            {
                std::locale loc;
                for (auto&& elem : args)
                    elem = std::tolower(elem, loc);
            }
            if (args.empty())
            {
                if (cmd->isEmptyArgsAllowed())
                    cmd->Execute(args);
                else
                    Info("{} {}", cmd->GetName(), cmd->Status());
            }
            else
                cmd->Execute(args);
        }
        else
        {
            Info("Command is disabled: {}", cmd->GetName());
            return false;
        }
    else
    {
        Warning("Unknown command.");
        return false;
    }
    return true;
}

bool CC_Container::ExecuteBool(CC_Bool* cmd, bool value) const
{
    if (cmd)
    {
        if (cmd->isEnabled())
            cmd->Execute(value);
        else
        {
            Info("Command is disabled: {}", cmd->GetName());
            return false;
        }
    }
    else
    {
        Warning("Unknown command.");
        return false;
    }
    return true;
}

ConsoleCommand* CC_Container::GetCommand(const std::string& cmd) const
{
    auto e = CommandsContainer.find(cmd);
    if (e != CommandsContainer.end())
        return e->second;
    return nullptr;
}

bool CC_Container::GetBool(const std::string& cmd) const
{
    ConsoleCommand* command = GetCommand(cmd);
    CC_Bool* b = dynamic_cast<CC_Bool*>(command);
    if (b)
        return b->GetValue();
    return false;
}

bool CC_Container::GetBool(CC_Bool* cmd)
{
    if (cmd)
        return cmd->GetValue();
    return false;
}

void CC_Container::AddCommand(ConsoleCommand* cc)
{
    CommandsContainer[cc->GetName()] = cc;
}

void CC_Container::RemoveCommand(ConsoleCommand* cc)
{
    if (!CommandsContainer.empty())
    {
        auto e = CommandsContainer.find(cc->GetName());
        if (e != CommandsContainer.end())
            CommandsContainer.erase(e);
    }
}

void CC_Container::Destroy()
{
    CommandsContainer.clear();
}
#pragma endregion ConsoleCommand Container

#pragma region Basic ConsoleCommand
ConsoleCommand::ConsoleCommand(std::string _name) : Name(_name), Enabled(true), LowerCaseArgs(true), AllowEmptyArgs(false), AllowSaving(true)
{
    CommandsCache.reserve(LRUCount + 1);
    CommandsCache.erase(CommandsCache.begin(), CommandsCache.end());
}

ConsoleCommand::~ConsoleCommand()
{
    if (ConsoleCommands)
        ConsoleCommands->RemoveCommand(this);
}

std::string ConsoleCommand::GetName() const
{
    return Name;
}

bool ConsoleCommand::isEnabled() const
{
    return Enabled;
}

bool ConsoleCommand::isLowerCaseArgs() const
{
    return LowerCaseArgs;
}

bool ConsoleCommand::isEmptyArgsAllowed() const
{
    return AllowEmptyArgs;
}

bool ConsoleCommand::isSavingAllowed() const
{
    return AllowSaving;
}

std::string ConsoleCommand::Status()
{
    return "no value";
}

std::string ConsoleCommand::Info()
{
    return "basic ConsoleCommand class";
}

std::string ConsoleCommand::Syntax()
{
    return "no arguments";
}

void ConsoleCommand::InvalidSyntax(std::string args)
{
    Warning("Invalid syntax in call [{} {}]", Name, args);
    Warning("Valid arguments: {}", Syntax());
}

std::string ConsoleCommand::Save()
{
    return GetName() + " " + Status();
}

void ConsoleCommand::AddCommandToCache(std::string&& cmd)
{
    if (cmd.size() == 0 || AllowEmptyArgs)
    {
        return;
    }
    bool isDublicate = std::find(CommandsCache.begin(), CommandsCache.end(), cmd) != CommandsCache.end();
    if (!isDublicate)
    {
        CommandsCache.push_back(cmd);
        if (CommandsCache.size() > LRUCount)
        {
            CommandsCache.erase(CommandsCache.begin());
        }
    }
}
#pragma endregion Basic ConsoleCommand

#pragma region ConsoleCommand Boolean
CC_Bool::CC_Bool(std::string _name, bool _value, bool _enabled) : super(_name), value(_value)
{
    Enabled = _enabled;
}

void CC_Bool::Execute(const std::string& args)
{
    bool v;
    if (args.compare("on") == 0 || args.compare("true") == 0 || args.compare("1") == 0)
    {
        v = true;
    }
    else if (args.compare("off") !=0 || args.compare("false") != 0 || args.compare("0") != 0)
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

void CC_Bool::Execute(bool _value) const
{
    value = _value;
}

std::string CC_Bool::Info()
{
    return "boolean value";
}

std::string CC_Bool::Syntax()
{
    return "on/off, true/false, 1/0";
}
std::string CC_Bool::Status()
{
    return value ? "on" : "off";
}

bool CC_Bool::GetValue() const
{
    return value;
}

#pragma endregion ConsoleCommand Boolean

#pragma region ConsoleCommand Toggle
XDay::CC_Toggle::CC_Toggle(std::string _name, bool _value, bool _enabled /*= true*/) : super(_name), value(_value)
{
    AllowEmptyArgs = true;
    AllowSaving = false;
    Enabled = _enabled;
}

void CC_Toggle::Execute(const std::string& args)
{
    value = !value;
}

std::string CC_Toggle::Info()
{
    return "toggler";
}

std::string CC_Toggle::Status()
{
    return value ? "on" : "off";
}


#pragma endregion ConsoleCommand Toggle

#pragma region ConsoleCommand String
CC_String::CC_String(std::string _name, std::string _value, unsigned _size, bool _enabled)
    : super(_name), value(_value), size(_size)
{
    Enabled = _enabled;
}

void CC_String::Execute(const std::string& args)
{
    if (args.length() > size)
    {
        InvalidSyntax(args);
        return;
    }
    value = args;
}

std::string CC_String::Info()
{
    return "string value";
}

std::string CC_String::Syntax()
{
    return fmt::format("max size is {}", size);
}

std::string CC_String::Status()
{
    return value;
}

#pragma endregion ConsoleCommand String

#pragma region ConsoleCommand Value Template
template<class T>
CC_Value<T>::CC_Value(std::string _name, T& _value, T const _min, T const _max)
    : super(_name), value(_value), min(_min), max(_max) {}

template<class T>
std::string CC_Value<T>::Syntax()
{
    return fmt::format("range [{}, {}]", min, max);
}
#pragma endregion ConsoleCommand Value Template

#pragma region ConsoleCommand Integer
CC_Integer::CC_Integer(std::string _name, int& _value, int const _min, int const _max, bool _enabled)
    : super(_name, _value, _min, _max)
{
    Enabled = _enabled;
}

void CC_Integer::Execute(const std::string& args)
{
    int v;
    if (1 != sscanf_s(args.c_str(), "%d", &v) || v < min || v > max)
    {
        InvalidSyntax(args);
        return;
    }
        
    value = v;
}

std::string CC_Integer::Info()
{
    return "integer value";
}

std::string CC_Integer::Status()
{
    return std::to_string(value);
}
#pragma endregion ConsoleCommand Integer

#pragma region ConsoleCommand Float
CC_Float::CC_Float(std::string _name, float& _value, float const _min, float const _max, bool _enabled)
    : super(_name, _value, _min, _max)
{
    Enabled = _enabled;
}

void CC_Float::Execute(const std::string& args)
{
    float v = min;
    if (sscanf_s(args.c_str(), "%f", &v) == 0 || v < min || v > max)
    {
        InvalidSyntax(args);
        return;
    }
    value = v;
}

std::string CC_Float::Info()
{
    return "float value";
}

std::string CC_Float::Status()
{
    return std::to_string(value);
}
#pragma endregion ConsoleCommand Float

#pragma region ConsoleCommand Double
CC_Double::CC_Double(std::string _name, double& _value, double const _min, double const _max, bool _enabled)
    : super(_name, _value, _min, _max)
{
    Enabled = _enabled;
}

void CC_Double::Execute(const std::string& args)
{
    double v = min;
    if (sscanf_s(args.c_str(), "%lf", &v) == 0 || v < min || v > max)
    {
        InvalidSyntax(args);
        return;
    }
    value = v;
}

std::string CC_Double::Info()
{
    return "double value";
}

std::string CC_Double::Status()
{
    return std::to_string(value);
}
#pragma endregion ConsoleCommand Double

#pragma region ConsoleCommand Function Call
CC_FunctionCall::CC_FunctionCall(std::string _name, void (*_func)(const std::string&), bool _AllowEmptyArgs, bool _enabled) : super(_name)
{
    AllowEmptyArgs = _AllowEmptyArgs;
    AllowSaving = false;
    Enabled = _enabled;
    function = _func;
}

void CC_FunctionCall::Execute(const std::string& args)
{
    if (function)
        function(args);
}

std::string CC_FunctionCall::Info()
{
    return "function call";
}

#pragma endregion ConsoleCommand Function Call
