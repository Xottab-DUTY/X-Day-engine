#include <sstream>
#include <fstream>
#include <locale>

#include "xdCore.hpp"
#include "ConsoleCommand.hpp"

#pragma region ConsoleCommand Container
bool xdCC_Container::Execute(std::string cmd)
{
    std::string cmd_str, cmd_val;

    std::istringstream buffer(cmd);
    buffer >> cmd_str >> cmd_val;

    xdConsoleCommand* command(GetCommand(cmd_str));
    if (command && command->isEnabled())
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
                command->Execute(cmd_val.c_str());
            else
                ConsoleMsg("Console: {} {}", command->GetName(), command->Status());
        }
        else
            command->Execute(cmd_val.c_str());
    }
    else
    {
        ConsoleMsg("Unknown command: {}", cmd_str);
        return false;
    }
    return true;
}

void xdCC_Container::ExecuteConfig(std::string filename)
{
    filename.insert(0, "config_load ");
    Execute(filename);
}

xdConsoleCommand* xdCC_Container::GetCommand(std::string cmd) const
{
    auto e = CommandsContainer.find(cmd);
    if (e != CommandsContainer.end())
    {
        return e->second;
    }
    else
    {
        Console->Log("CC not found");
        return nullptr;
    }
        
}

bool xdCC_Container::GetBool(std::string cmd) const
{
    xdConsoleCommand* command = GetCommand(cmd);
    xdCC_Bool* b = dynamic_cast<xdCC_Bool*>(command);
    if (b)
        return b->GetValue();
    return false;
}

void xdCC_Container::AddCommand(xdConsoleCommand* cc)
{
    CommandsContainer[cc->GetName()] = cc;
}

void xdCC_Container::RemoveCommand(xdConsoleCommand* cc)
{
    auto e = CommandsContainer.find(cc->GetName());
    if (e != CommandsContainer.end())
        CommandsContainer.erase(e);
}

void xdCC_Container::Destroy()
{
    CommandsContainer.clear();
}
#pragma endregion ConsoleCommand Container

#pragma region Basic ConsoleCommand
xdConsoleCommand::xdConsoleCommand(std::string _name) : Name(_name), Enabled(true), LowerCaseArgs(true), AllowEmptyArgs(false)
{
    CommandsCache.reserve(LRUCount + 1);
    CommandsCache.erase(CommandsCache.begin(), CommandsCache.end());
}

xdConsoleCommand::~xdConsoleCommand()
{
    if (ConsoleCommands)
        ConsoleCommands->RemoveCommand(this);
}

std::string xdConsoleCommand::GetName()
{
    return Name;
}

bool xdConsoleCommand::isEnabled()
{
    return Enabled;
}

bool xdConsoleCommand::isLowerCaseArgs()
{
    return LowerCaseArgs;
}

bool xdConsoleCommand::isEmptyArgsAllowed()
{
    return AllowEmptyArgs;
}

std::string xdConsoleCommand::Status()
{
    return "No status.";
}

std::string xdConsoleCommand::Info()
{
    return "Basic ConsoleCommand class.";
}

std::string xdConsoleCommand::Syntax()
{
    return "No arguments.";
}

void xdConsoleCommand::InvalidSyntax(std::string args)
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
xdCC_Bool::xdCC_Bool(std::string _name, bool& _value) : super(_name), value(_value) {}

void xdCC_Bool::Execute(std::string args)
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

std::string xdCC_Bool::Info()
{
    return "Boolean value.";
}

std::string xdCC_Bool::Syntax()
{
    return "on/off, true/false, 1/0";
}
std::string xdCC_Bool::Status()
{
    return value ? "on" : "off";
}

const bool xdCC_Bool::GetValue() const
{
    return value;
}

#pragma endregion ConsoleCommand Boolean

#pragma region ConsoleCommand Toggle
xdCC_Toggle::xdCC_Toggle(std::string _name, bool& _value) : super(_name), value(_value)
{
    AllowEmptyArgs = true;
}

void xdCC_Toggle::Execute(std::string args)
{
    value = !value;
    ConsoleMsg("ConsoleCommand {} toggled ({})", Name, value);
}

std::string xdCC_Toggle::Info()
{
    return "Command with no arguments needed";
}
std::string xdCC_Toggle::Status()
{
    return value ? "on" : "off";
}


#pragma endregion ConsoleCommand Toggle

#pragma region ConsoleCommand String
xdCC_String::xdCC_String(std::string _name, std::string&& _value, unsigned _size)
    : super(_name), value(_value), size(_size)
{}

void xdCC_String::Execute(std::string args)
{
    if (args.length() > size)
    {
        InvalidSyntax(args);
        return;
    }
    value = args;
}

std::string xdCC_String::Info()
{
    return "String value.";
}

std::string xdCC_String::Syntax()
{
    return fmt::format("Max size is %d", size);
}

std::string xdCC_String::Status()
{
    return value;
}

#pragma endregion ConsoleCommand String

#pragma region ConsoleCommand Value Template
template<class T>
inline xdCC_Value<T>::xdCC_Value(std::string _name, T& _value, T const _min, T const _max)
    : super(_name), value(_value), min(_min), max(_max) {}
#pragma endregion ConsoleCommand Value Template

#pragma region ConsoleCommand Unsigned Integer
xdCC_Unsigned::xdCC_Unsigned(std::string _name, unsigned& _value, unsigned const _min, unsigned const _max)
    : super(_name, _value, _min, _max) {}

void xdCC_Unsigned::Execute(std::string args)
{
    unsigned v;
    if (1 != sscanf_s(args.c_str(), "%d", &v) || v < min || v > max)
    {
        InvalidSyntax(args);
        return;
    }
        
    value = v;
}

std::string xdCC_Unsigned::Info()
{
    return "Integer value.";
}

std::string xdCC_Unsigned::Syntax()
{
    return fmt::format("Range [{}, {}]", min, max);
}

std::string xdCC_Unsigned::Status()
{
    return std::to_string(value);
}
#pragma endregion ConsoleCommand Unsigned Integer

#pragma region ConsoleCommand Float
xdCC_Float::xdCC_Float(std::string _name, float& _value, float const _min, float const _max)
    : super(_name, _value, _min, _max) {}

void xdCC_Float::Execute(std::string args)
{
    float v = min;
    if (1 != sscanf_s(args.c_str(), "%f", &v) || v < min || v > max)
    {
        InvalidSyntax(args);
        return;
    }
    value = v;
}

std::string xdCC_Float::Info()
{
    return "Float value.";
}
std::string xdCC_Float::Syntax()
{
    return fmt::format("Range [{}, {}]", min, max);
}
std::string xdCC_Float::Status()
{
    return std::to_string(value);
}
#pragma endregion ConsoleCommand Float

#pragma region ConsoleCommand Config Loader
xdCC_LoadConfig::xdCC_LoadConfig(std::string _name) : super(_name)
{
    LowerCaseArgs = false;
}

void xdCC_LoadConfig::Execute(std::string args)
{
    ConsoleMsg("Loading config file {}...", args);
    std::ifstream config_file(args);
    std::string line;

    if (config_file.is_open())
        while (std::getline(config_file, line))
            ConsoleCommands->Execute(line);
    else
        ConsoleMsg("Failed to open config file {}", args);

    config_file.close();
}
#pragma endregion ConsoleCommand Config Loader

bool r_fullscreen = false;

void RegisterConsoleCommands()
{
    CMD1(xdCC_LoadConfig, LoadConfigCC, "config_load");
    CMD2(xdCC_Bool, FullscreenCC, "r_fullscreen", r_fullscreen);
}