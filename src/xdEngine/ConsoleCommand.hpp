#pragma once
#include <string>
#include <map>
#include <filesystem>
namespace filesystem = std::experimental::filesystem::v1;

#include <fmt/format.h>

#include "xdEngine_impexp.inl"
#include "Console.hpp"

class XDAY_API ConsoleCommand;

#pragma region ConsoleCommand Container
class XDAY_API CC_Container
{
public:
    std::map<std::string, ConsoleCommand*> CommandsContainer;

    bool Execute(std::string cmd);
    void ExecuteConfig(std::string filename);
    ConsoleCommand* GetCommand(std::string cmd) const;
    bool GetBool(std::string cmd) const;

    void AddCommand(ConsoleCommand* cc);
    void RemoveCommand(ConsoleCommand* cc);

    void Destroy();
};
#pragma endregion ConsoleCommand Container

XDAY_API extern CC_Container* ConsoleCommands;

#pragma region ConsoleCommand Adding Macros
#define CMD0(cls, ccname)                   { static cls ccname();              ConsoleCommands->AddCommand(&ccname); }
#define CMD1(cls, ccname, p1)               { static cls ccname(p1);            ConsoleCommands->AddCommand(&ccname); }
#define CMD2(cls, ccname, p1, p2)           { static cls ccname(p1,p2);         ConsoleCommands->AddCommand(&ccname); }
#define CMD3(cls, ccname, p1, p2, p3)       { static cls ccname(p1,p2,p3);      ConsoleCommands->AddCommand(&ccname); }
#define CMD4(cls, ccname, p1, p2, p3, p4)   { static cls ccname(p1,p2,p3,p4);   ConsoleCommands->AddCommand(&ccname); }
#pragma endregion ConsoleCommand Adding Macros

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
    CC_Bool(std::string _name, bool& _value);

    virtual void Execute(std::string args) override;

    virtual std::string Info() override;
    virtual std::string Syntax() override;
    virtual std::string Status() override;

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
    CC_Toggle(std::string _name, bool& _value);

    virtual void Execute(std::string args) override;

    virtual std::string Info() override;
    virtual std::string Status() override;

protected:
    bool& value;
};
#pragma endregion ConsoleCommand Toggle

#pragma region ConsoleCommand String
class XDAY_API CC_String : public ConsoleCommand
{
    using super = ConsoleCommand;
public:
    CC_String(std::string _name, std::string&& _value, unsigned _size);

    virtual void Execute(std::string args) override;

    virtual std::string Info() override;
    virtual std::string Syntax() override;
    virtual std::string Status() override;

protected:
    std::string value;
    unsigned size;
};
#pragma endregion ConsoleCommand String

#pragma region ConsoleCommand Value Template
template<class T>
class XDAY_API CC_Value : public ConsoleCommand
{
    using super = ConsoleCommand;
public:
    inline CC_Value(std::string _name, T& _value, T const _min, T const _max);
protected:
    T& value;
    T min;
    T max;
};
#pragma endregion ConsoleCommand Value Template

#pragma region ConsoleCommand Unsigned Integer
class XDAY_API CC_Unsigned : public CC_Value<unsigned>
{
    using super = CC_Value<unsigned>;

public:
    CC_Unsigned(std::string _name, unsigned& _value, unsigned const _min, unsigned const _max);

    virtual void Execute(std::string args) override;

    virtual std::string Info() override;
    virtual std::string Syntax() override;
    virtual std::string Status() override;
};
#pragma endregion ConsoleCommand Unsigned Integer

#pragma region ConsoleCommand Float
class XDAY_API CC_Float : public CC_Value<float>
{
    using super = CC_Value<float>;

public:
    CC_Float(std::string _name, float& _value, float const _min, float const _max);

    virtual void Execute(std::string args) override;

    virtual std::string Info() override;
    virtual std::string Syntax() override;
    virtual std::string Status() override;
};
#pragma endregion ConsoleCommand Float

#pragma region ConsoleCommand Function Call
class XDAY_API CC_FCall : public ConsoleCommand
{
    using super = ConsoleCommand;

public:
    CC_FCall(std::string _name, void(*_func)(std::string), bool _AllowEmptyArgs);
    void Execute(std::string args) override;

    std::string Info() override;
protected:
    void (*function)(std::string);
};
#pragma endregion ConsoleCommand Function Call

void RegisterConsoleCommands();
