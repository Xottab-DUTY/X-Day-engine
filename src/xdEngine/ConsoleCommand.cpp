#include <sstream>
#include <fstream>
#include <locale>

#include <GLFW/glfw3.h>

#include "xdCore.hpp"
#include "xdEngine.hpp"
#include "ConsoleCommand.hpp"

#pragma region ConsoleCommand Container
bool CC_Container::Execute(std::string cmd) const
{
    std::string cmd_str, cmd_val;

    std::istringstream buffer(cmd);
    buffer >> cmd_str >> cmd_val;

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
	                command->Execute(cmd_val.c_str());
	            else
	                ConsoleMsg("{} {}", command->GetName(), command->Status());
	        }
	        else
	            command->Execute(cmd_val.c_str());
        } 
        else
        {
            ConsoleMsg("Command is disabled: {}", cmd_str);
            return false;
        }
    }
    else
    {
        ConsoleMsg("Unknown command: {}", cmd_str);
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
                ConsoleMsg("{} {}", cmd->GetName(), cmd->Status());
        }
        else
        {
            ConsoleMsg("Command is disabled: {}", cmd->GetName());
            return false;
        }
            
    }
    else
    {
        Console->Log("Unknown command.");
        return false;
    }
    return true;
}

bool CC_Container::Execute(ConsoleCommand* cmd, std::string args) const
{
    if (cmd)
    {
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
                    cmd->Execute(args.c_str());
                else
                    ConsoleMsg("{} {}", cmd->GetName(), cmd->Status());
            }
            else
                cmd->Execute(args.c_str());
        }
        else
        {
            ConsoleMsg("Command is disabled: {}", cmd->GetName());
            return false;
        }
    }
    else
    {
        Console->Log("Unknown command.");
        return false;
    }
    return true;
}

void CC_Container::ExecuteConfig(std::string filename)
{
    filename.insert(0, "config_load ");
    Execute(filename);
}

ConsoleCommand* CC_Container::GetCommand(std::string cmd) const
{
    auto e = CommandsContainer.find(cmd);
    if (e != CommandsContainer.end())
    {
        return e->second;
    }
    else
    {
        return nullptr;
    }
        
}

bool CC_Container::GetBool(std::string cmd) const
{
    ConsoleCommand* command = GetCommand(cmd);
    CC_Bool* b = dynamic_cast<CC_Bool*>(command);
    if (b)
        return b->GetValue();
    return false;
}

void CC_Container::AddCommand(ConsoleCommand* cc)
{
    CommandsContainer[cc->GetName()] = cc;
}

void CC_Container::RemoveCommand(ConsoleCommand* cc)
{
    auto e = CommandsContainer.find(cc->GetName());
    if (e != CommandsContainer.end())
        CommandsContainer.erase(e);
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

std::string ConsoleCommand::GetName()
{
    return Name;
}

bool ConsoleCommand::isEnabled()
{
    return Enabled;
}

bool ConsoleCommand::isLowerCaseArgs()
{
    return LowerCaseArgs;
}

bool ConsoleCommand::isEmptyArgsAllowed()
{
    return AllowEmptyArgs;
}

bool ConsoleCommand::isSavingAllowed()
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
    ConsoleMsg("Invalid syntax in call [{} {}]", Name, args);
    ConsoleMsg("Valid arguments: {}", Syntax());
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
CC_Bool::CC_Bool(std::string _name, bool& _value) : super(_name), value(_value) {}

void CC_Bool::Execute(std::string args)
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

const bool CC_Bool::GetValue() const
{
    return value;
}

#pragma endregion ConsoleCommand Boolean

#pragma region ConsoleCommand Toggle
CC_Toggle::CC_Toggle(std::string _name, bool& _value) : super(_name), value(_value)
{
    AllowEmptyArgs = true;
    AllowSaving = false;
}

void CC_Toggle::Execute(std::string args)
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
CC_String::CC_String(std::string _name, std::string _value, unsigned _size)
    : super(_name), value(_value), size(_size)
{}

void CC_String::Execute(std::string args)
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
CC_Integer::CC_Integer(std::string _name, int& _value, int const _min, int const _max)
    : super(_name, _value, _min, _max) {}

void CC_Integer::Execute(std::string args)
{
    unsigned v;
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
CC_Float::CC_Float(std::string _name, float& _value, float const _min, float const _max)
    : super(_name, _value, _min, _max) {}

void CC_Float::Execute(std::string args)
{
    float v = min;
    if (1 != sscanf_s(args.c_str(), "%f", &v) || v < min || v > max)
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
CC_Double::CC_Double(std::string _name, double& _value, double const _min, double const _max)
    : super(_name, _value, _min, _max) {}

void CC_Double::Execute(std::string args)
{
    float v = min;
    if (1 != sscanf_s(args.c_str(), "%f", &v) || v < min || v > max)
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
CC_FunctionCall::CC_FunctionCall(std::string _name, void (*_func)(std::string), bool _AllowEmptyArgs) : super(_name)
{
    AllowEmptyArgs = _AllowEmptyArgs;
    AllowSaving = false;
    function = _func;
}

void CC_FunctionCall::Execute(std::string args)
{
    if (function)
        function(args);
}

std::string CC_FunctionCall::Info()
{
    return "function call";
}

#pragma endregion ConsoleCommand Function Call

void CC_ConfigLoad(std::string args)
{
    ConsoleMsg("Loading config file {}...", args.empty() ? Console->ConfigFile.string() : args);
    std::ifstream config_file(args.empty() ? Console->ConfigFile : args);
    std::string line;

    if (config_file.is_open())
    {
        while (std::getline(config_file, line))
            ConsoleCommands->Execute(line);
        ConsoleMsg("Loaded config file {}", args.empty() ? Console->ConfigFile.string() : args);
    }
    else
        ConsoleMsg("Failed to open config file {}", args.empty() ? Console->ConfigFile.string() : args);

    config_file.close();
}

void CC_ConfigSave(std::string args)
{
    ConsoleMsg("Saving config file {}...", args.empty() ? Console->ConfigFile.string() : args);
    std::ofstream f(args.empty() ? Console->ConfigFile : args);
    for (auto str : ConsoleCommands->CommandsContainer)
    {
        auto CommandToSave = str.second;
        if (CommandToSave->isSavingAllowed())
            f << CommandToSave->Save() << std::endl;
    }
    f.close();
    ConsoleMsg("Saved config file {}", args.empty() ? Console->ConfigFile.string() : args);
}

void CC_Help(std::string args)
{
    ConsoleCommand* CommandToHelp;
    if (!args.empty())
    {
        CommandToHelp = ConsoleCommands->GetCommand(args);
        if (CommandToHelp)
            ConsoleMsg("{} : {}. Current value: {}. Syntax: {}", CommandToHelp->GetName(), CommandToHelp->Info(), CommandToHelp->Status(), CommandToHelp->Syntax())
        else
            Console->Log("Command not found.");
    }
    else
    {
        Console->Log("Available commands:");
        for (auto str : ConsoleCommands->CommandsContainer)
        {
            CommandToHelp = str.second;
            ConsoleMsg("{} : {}. Current value: {}. Syntax: {}", CommandToHelp->GetName(), CommandToHelp->Info(), CommandToHelp->Status(), CommandToHelp->Syntax());
        }
    }
}

void CC_Exit(std::string args)
{
    glfwSetWindowShouldClose(Engine.window, GLFW_TRUE);
}

bool r_fullscreen = false;

int int_test = 1;
float float_test = 1.0;
double double_test = 1.0;
bool toggle_test = false;
std::string string_test = "test";

void RegisterConsoleCommands()
{
    /*
        A bit of help:
        
        CMDA – Adds already declared function

        CC_FunctionCall – CMD3(CC_FunctionCall, command_name_in_this_file, "command_name_in_console", function_to_call, is_empty_args_allowed);

        CC_Bool         – CMD2(CC_Bool,         command_name_in_this_file, "command_name_in_console", variable_to_change);

        CC_Toggle       – CMD2(CC_Toggle,       command_name_in_this_file, "command_name_in_console", variable_to_change);

        CC_Integer      – CMD4(CC_Integer,      command_name_in_this_file, "command_name_in_console", variable_to_change, minimum_value, maximum_value);

        CC_Float        – CMD4(CC_Float,        command_name_in_this_file, "command_name_in_console", variable_to_change, minimum_value, maximum_value);

        CC_Double       – CMD4(CC_Double,       command_name_in_this_file, "command_name_in_console", variable_to_change, minimum_value, maximum_value);

        CC_String       – CMD3(CC_String,       command_name_in_this_file, "command_name_in_console", variable_to_change, max_string_size);
    */
    CMDA(ExitCC);
    CMDA(QuitCC);

    CMDA(HelpCC);

    CMDA(ConfigLoadCC);
    CMDA(ConfigSaveCC);

    CMD2(CC_Bool, FullscreenCC, "r_fullscreen", r_fullscreen);

    CMD2(CC_Toggle, toggle_testCC, "toggle_test", toggle_test);
    CMD4(CC_Integer, int_testCC, "int_test", int_test, 1, 10);
    CMD4(CC_Float, float_testCC, "float_test", float_test, 1.0, 10.0);
    CMD4(CC_Double, double_testCC, "double_test", double_test, 1.0, 10.0);
    CMD3(CC_String, string_testCC, "string_test", string_test, 512);
}
