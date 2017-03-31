#include <fstream>

#include "ConsoleCommand.hpp"

#pragma region ConsoleCommand Container
void xdCC_Container::AddCommand(xdConsoleCommand* cmdName)
{
    CommandsContainer[cmdName->GetName()] = cmdName;
}

void xdCC_Container::RemoveCommand(xdConsoleCommand* cmdName)
{
    
}
#pragma endregion ConsoleCommand Container

#pragma region Basic ConsoleCommand
xdConsoleCommand::xdConsoleCommand(const char* _name) : Name(_name), Enabled(true), LowerCaseArgs(true), AllowEmptyArgs(false)
{
    CommandsCache.reserve(LRUCount + 1);
    CommandsCache.erase(CommandsCache.begin(), CommandsCache.end());
}

xdConsoleCommand::~xdConsoleCommand()
{
    if (ConsoleCommands)
        ConsoleCommands->RemoveCommand(this);
}

const char* xdConsoleCommand::GetName()
{
    return Name;
}

const char* xdConsoleCommand::Status()
{
    return "No status.";
}

const char* xdConsoleCommand::Info()
{
    return "Basic ConsoleCommand class.";
}

const char* xdConsoleCommand::Syntax()
{
    return "No arguments.";
}

void xdConsoleCommand::InvalidSyntax(const char* args)
{
    ConsoleMsg("Invalid syntax in call [{} {}]", Name, args);
    ConsoleMsg("Valid arguments: {}", Syntax());
}

void xdConsoleCommand::Save(filesystem::path&& p)
{
    std::ofstream f(p);
    f << GetName() << " " << Status() << std::endl;
    f.close();
}

void xdConsoleCommand::AddCommandToCache(std::string&& cmd)
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
xdCC_Bool::xdCC_Bool(const char* _name, bool& _value) : super(_name), value(_value) {}

void xdCC_Bool::Execute(const char* args)
{
    bool v;
    if (!strcmp(args, "on") || !strcmp(args, "true") || !strcmp(args, "1"))
    {
        v = true;
    }
    else if (!strcmp(args, "off") || !strcmp(args, "false") || !strcmp(args, "0"))
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

const char* xdCC_Bool::Info()
{
    return "Boolean value.";
}

const char* xdCC_Bool::Syntax()
{
    return "on/off, true/false, 1/0";
}
const char* xdCC_Bool::Status()
{
    return value ? "on" : "off";
}
#pragma endregion ConsoleCommand Boolean

#pragma region ConsoleCommand Toggle
xdCC_Toggle::xdCC_Toggle(const char* _name, bool& _value) : super(_name), value(_value)
{
    AllowEmptyArgs = true;
}

void xdCC_Toggle::Execute(const char* args)
{
    value = !value;
    ConsoleMsg("ConsoleCommand {} toggled ({})", Name, value);
}

const char* xdCC_Toggle::Info()
{
    return "Command with no arguments needed";
}
const char* xdCC_Toggle::Status()
{
    return value ? "on" : "off";
}


#pragma endregion ConsoleCommand Toggle

#pragma region ConsoleCommand String
xdCC_String::xdCC_String(const char* _name, std::string&& _value, unsigned _size)
    : super(_name), value(_value), size(_size)
{}

void xdCC_String::Execute(const char* args)
{
    if (strlen(args) > size)
    {
        InvalidSyntax(args);
        return;
    }
    value = args;
}

const char* xdCC_String::Info()
{
    return "String value.";
}

const char* xdCC_String::Syntax()
{
    return fmt::format("Max size is %d", size).c_str();
}

const char* xdCC_String::Status()
{
    return value.c_str();
}

#pragma endregion ConsoleCommand String

#pragma region ConsoleCommand Value Template
template<class T>
inline xdCC_Value<T>::xdCC_Value(const char* _name, T& _value, T const _min, T const _max)
    : super(_name), value(_value), min(_min), max(_max) {}
#pragma endregion ConsoleCommand Value Template

#pragma region ConsoleCommand Unsigned Integer
xdCC_Unsigned::xdCC_Unsigned(const char* _name, unsigned& _value, unsigned const _min, unsigned const _max)
    : super(_name, _value, _min, _max) {}

void xdCC_Unsigned::Execute(const char* args)
{
    unsigned v;
    if (1 != sscanf_s(args, "%d", &v) || v < min || v > max)
    {
        InvalidSyntax(args);
        return;
    }
        
    value = v;
}

const char* xdCC_Unsigned::Info()
{
    return "Integer value.";
}

const char* xdCC_Unsigned::Syntax()
{
    return fmt::format("Range [{}, {}]", min, max).c_str();
}

const char* xdCC_Unsigned::Status()
{
    return std::to_string(value).c_str();
}
#pragma endregion ConsoleCommand Unsigned Integer

#pragma region ConsoleCommand Float
xdCC_Float::xdCC_Float(const char* _name, float& _value, float const _min, float const _max)
    : super(_name, _value, _min, _max) {}

void xdCC_Float::Execute(const char* args)
{
    float v = min;
    if (1 != sscanf_s(args, "%f", &v) || v < min || v > max)
    {
        InvalidSyntax(args);
        return;
    }
    value = v;
}

const char* xdCC_Float::Info()
{
    return "Float value.";
}
const char* xdCC_Float::Syntax()
{
    return fmt::format("Range [{}, {}]", min, max).c_str();
}
const char* xdCC_Float::Status()
{
    return std::to_string(value).c_str();
}
#pragma endregion ConsoleCommand Float
