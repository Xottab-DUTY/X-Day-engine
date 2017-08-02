#pragma once
#ifndef ConsoleCommand_hpp__
#define ConsoleCommand_hpp__

#include <string>
#include <map>
#include <filesystem>
namespace filesystem = std::experimental::filesystem;

#include <fmt/format.h>

#include "xdEngine_impexp.inl"
#include "Console.hpp"

#pragma region ConsoleCommand Adding Macros
#define CMDA(ccname) { ConsoleCommands->AddCommand(&ccname); }

#define CMD0(cls, ccname)                       { static cls ccname();                  ConsoleCommands->AddCommand(&ccname); }
#define CMD1(cls, ccname, p1)                   { static cls ccname(p1);                ConsoleCommands->AddCommand(&ccname); }
#define CMD2(cls, ccname, p1, p2)               { static cls ccname(p1,p2);             ConsoleCommands->AddCommand(&ccname); }
#define CMD3(cls, ccname, p1, p2, p3)           { static cls ccname(p1,p2,p3);          ConsoleCommands->AddCommand(&ccname); }
#define CMD4(cls, ccname, p1, p2, p3, p4)       { static cls ccname(p1,p2,p3,p4);       ConsoleCommands->AddCommand(&ccname); }
#define CMD5(cls, ccname, p1, p2, p3, p4, p5)   { static cls ccname(p1,p2,p3,p4,p5);    ConsoleCommands->AddCommand(&ccname); }
#pragma endregion ConsoleCommand Adding Macros

namespace XDay
{
#pragma region Basic ConsoleCommand
class XDAY_API ConsoleCommand
{
public:
    friend class xdConsole;
protected:
    std::string Name;
    bool Enabled;
    bool LowerCaseArgs;
    bool AllowEmptyArgs;
    bool AllowSaving;

    // Commands LRU cache
    unsigned LRUCount = 10; // Max latest commands to remember
    std::vector<std::string> CommandsCache;

public:
    ConsoleCommand(std::string _name);
    virtual ~ConsoleCommand();

    virtual void Execute(std::string args) = 0;

    std::string GetName();
    bool isEnabled();
    bool isLowerCaseArgs();
    bool isEmptyArgsAllowed();
    bool isSavingAllowed();

    virtual std::string Status();
    virtual std::string Info();
    virtual std::string Syntax();
    void InvalidSyntax(std::string args);

    virtual std::string Save();

    virtual void AddCommandToCache(std::string&& cmd);
};
#pragma endregion Basic ConsoleCommand

#pragma region ConsoleCommand Boolean
class XDAY_API CC_Bool : public ConsoleCommand
{
    using super = ConsoleCommand;

public:
    CC_Bool(std::string _name, bool& _value, bool _enabled = true);

    void Execute(std::string args) override;
    void Execute(bool _value) const;

    std::string Info() override;
    std::string Syntax() override;
    std::string Status() override;

    const bool GetValue() const;

protected:
    bool& value;
};
#pragma endregion ConsoleCommand Boolean

#pragma region ConsoleCommand Toggle
class XDAY_API CC_Toggle : public ConsoleCommand
{
    using super = ConsoleCommand;

public:
    CC_Toggle(std::string _name, bool& _value, bool _enabled = true);

    void Execute(std::string args) override;

    std::string Info() override;
    std::string Status() override;

protected:
    bool& value;
};
#pragma endregion ConsoleCommand Toggle

#pragma region ConsoleCommand String
class XDAY_API CC_String : public ConsoleCommand
{
    using super = ConsoleCommand;
public:
    CC_String(std::string _name, std::string _value, unsigned _size, bool _enabled = true);

    void Execute(std::string args) override;

    std::string Info() override;
    std::string Syntax() override;
    std::string Status() override;

protected:
    std::string value;
    unsigned size;
};
#pragma endregion ConsoleCommand String

#pragma region ConsoleCommand Value Template
template <class T>
class XDAY_API CC_Value : public ConsoleCommand
{
    using super = ConsoleCommand;

public:
    CC_Value(std::string _name, T& _value, T const _min, T const _max);

    std::string Syntax() override;

protected:
    T& value;
    T min;
    T max;
};
#pragma endregion ConsoleCommand Value Template

#pragma region ConsoleCommand Integer
class XDAY_API CC_Integer : public CC_Value<int>
{
    using super = CC_Value<int>;

public:
    CC_Integer(std::string _name, int& _value, int const _min, int const _max, bool _enabled = true);

    void Execute(std::string args) override;

    std::string Info() override;
    std::string Status() override;
};
#pragma endregion ConsoleCommand Integer

#pragma region ConsoleCommand Float
class XDAY_API CC_Float : public CC_Value<float>
{
    using super = CC_Value<float>;

public:
    CC_Float(std::string _name, float& _value, float const _min, float const _max, bool _enabled = true);

    void Execute(std::string args) override;

    std::string Info() override;
    std::string Status() override;
};
#pragma endregion ConsoleCommand Float

#pragma region ConsoleCommand Double
class XDAY_API CC_Double : public CC_Value<double>
{
    using super = CC_Value<double>;

public:
    CC_Double(std::string _name, double& _value, double const _min, double const _max, bool _enabled = true);

    void Execute(std::string args) override;

    std::string Info() override;
    std::string Status() override;
};
#pragma endregion ConsoleCommand Double

#pragma region ConsoleCommand Function Call
class XDAY_API CC_FunctionCall : public ConsoleCommand
{
    using super = ConsoleCommand;

public:
    CC_FunctionCall(std::string _name, void (*_func)(std::string), bool _AllowEmptyArgs, bool _enabled = true);
    void Execute(std::string args) override;

    std::string Info() override;
protected:
    void (*function)(std::string args);
};
#pragma endregion ConsoleCommand Function Call

#pragma region ConsoleCommand Container
class XDAY_API CC_Container
{
public:
    std::map<std::string, ConsoleCommand*> CommandsContainer;

    bool Execute(std::string cmd) const;
    bool Execute(ConsoleCommand* cmd) const;
    bool Execute(ConsoleCommand* cmd, std::string args) const;

    bool ExecuteBool(CC_Bool* cmd, bool value) const;

    void ExecuteConfig(std::string filename) const;

    ConsoleCommand* GetCommand(std::string cmd) const;
    bool GetBool(std::string cmd) const;
    bool GetBool(CC_Bool* cmd) const;

    void AddCommand(ConsoleCommand* cc);
    void RemoveCommand(ConsoleCommand* cc);

    void Destroy();
};
#pragma endregion ConsoleCommand Container
}

XDAY_API extern XDay::CC_Container* ConsoleCommands;

extern bool GlobalFullscreen;

/*
A bit of help:

CC_FunctionCall ("command_name_in_console", function_to_call, is_empty_args_allowed);

CC_Bool         ("command_name_in_console", variable_to_change);

CC_Toggle       ("command_name_in_console", variable_to_change);

CC_Integer      ("command_name_in_console", variable_to_change, minimum_value, maximum_value);

CC_Float        ("command_name_in_console", variable_to_change, minimum_value, maximum_value);

CC_Double       ("command_name_in_console", variable_to_change, minimum_value, maximum_value);

CC_String       ("command_name_in_console", variable_to_change, max_string_size);
*/

#pragma region Console commands
// Don't put here not ready testing functions, use CMD0-4 in RegisterConsoleCommands() for them

namespace cc_functions
{
	void CC_Exit(std::string args);
	void CC_Help(std::string args);
	void CC_SystemCommand(std::string args);
	
	void CC_ConfigLoad(std::string args);
	void CC_ConfigSave(std::string args);
	
	void CC_FlushLog(std::string args);
}

static XDay::CC_FunctionCall ExitCC("exit", cc_functions::CC_Exit, true);
static XDay::CC_FunctionCall QuitCC("quit", cc_functions::CC_Exit, true);
static XDay::CC_FunctionCall HelpCC("help", cc_functions::CC_Help, true);
static XDay::CC_FunctionCall SystemCommandCC("system", cc_functions::CC_SystemCommand, false, Core.isGlobalDebug());

static XDay::CC_FunctionCall FlushLogCC("flush", cc_functions::CC_FlushLog, true);

static XDay::CC_FunctionCall ConfigLoadCC("config_load", cc_functions::CC_ConfigLoad, true);
static XDay::CC_FunctionCall ConfigSaveCC("config_save", cc_functions::CC_ConfigSave, true);

static XDay::CC_Bool FullscreenCC("r_fullscreen", GlobalFullscreen);

void RegisterConsoleCommands();
#pragma endregion Console commands

#endif // ConsoleCommand_h__
