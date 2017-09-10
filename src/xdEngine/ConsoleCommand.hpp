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

    virtual void Execute(const std::string& args) = 0;

    std::string GetName() const;
    bool isEnabled() const;
    bool isLowerCaseArgs() const;
    bool isEmptyArgsAllowed() const;
    bool isSavingAllowed() const;

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
    CC_Bool(std::string _name, bool _value, bool _enabled = true);

    void Execute(const std::string& args) override;
    void Execute(bool _value) const;

    std::string Info() override;
    std::string Syntax() override;
    std::string Status() override;

    bool GetValue() const;

protected:
    bool& value;
};
#pragma endregion ConsoleCommand Boolean

#pragma region ConsoleCommand Toggle
class XDAY_API CC_Toggle : public ConsoleCommand
{
    using super = ConsoleCommand;

public:
    CC_Toggle(std::string _name, bool _value, bool _enabled = true);

    void Execute(const std::string& args) override;

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

    void Execute(const std::string& args) override;

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

    void Execute(const std::string& args) override;

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

    void Execute(const std::string& args) override;

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

    void Execute(const std::string& args) override;

    std::string Info() override;
    std::string Status() override;
};
#pragma endregion ConsoleCommand Double

#pragma region ConsoleCommand Function Call
class XDAY_API CC_FunctionCall : public ConsoleCommand
{
    using super = ConsoleCommand;

public:
    CC_FunctionCall(std::string _name, void (*_func)(const std::string&), bool _AllowEmptyArgs, bool _enabled = true);
    void Execute(const std::string& args) override;

    std::string Info() override;
protected:
    void (*function)(const std::string& args);
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

    void ExecuteConfig(const filesystem::path& filename) const;

    ConsoleCommand* GetCommand(const std::string& cmd) const;
    bool GetBool(const std::string& cmd) const;
    bool GetBool(CC_Bool* cmd);

    void AddCommand(ConsoleCommand* cc);
    void RemoveCommand(ConsoleCommand* cc);

    void Destroy();
};
#pragma endregion ConsoleCommand Container
}

XDAY_API extern XDay::CC_Container* ConsoleCommands;

#endif // ConsoleCommand_h__
