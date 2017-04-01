#pragma once
#include <string>
#include <map>
#include <filesystem>
namespace filesystem = std::experimental::filesystem::v1;

#include <fmt/format.h>

#include "xdEngine_impexp.inl"
#include "Console.hpp"

class XDAY_API xdConsoleCommand;

#pragma region ConsoleCommand Container
class XDAY_API xdCC_Container
{
public:
    std::map<std::string, xdConsoleCommand*> CommandsContainer;

    bool Execute(std::string cmd);
    void ExecuteConfig(std::string filename);
    xdConsoleCommand* GetCommand(std::string cmd) const;
    bool GetBool(std::string cmd) const;

    void AddCommand(xdConsoleCommand* cc);
    void RemoveCommand(xdConsoleCommand* cc);

    void Destroy();
};
#pragma endregion ConsoleCommand Container

XDAY_API extern xdCC_Container* ConsoleCommands;

#pragma region ConsoleCommand Adding Macros
#define CMD0(cls, ccname)                   { static cls ccname();              ConsoleCommands->AddCommand(&ccname); }
#define CMD1(cls, ccname, p1)               { static cls ccname(p1);            ConsoleCommands->AddCommand(&ccname); }
#define CMD2(cls, ccname, p1, p2)           { static cls ccname(p1,p2);         ConsoleCommands->AddCommand(&ccname); }
#define CMD3(cls, ccname, p1, p2, p3)       { static cls ccname(p1,p2,p3);      ConsoleCommands->AddCommand(&ccname); }
#define CMD4(cls, ccname, p1, p2, p3, p4)   { static cls ccname(p1,p2,p3,p4);   ConsoleCommands->AddCommand(&ccname); }
#pragma endregion ConsoleCommand Adding Macros

#pragma region Basic ConsoleCommand
class XDAY_API xdConsoleCommand
{
public:
    friend class xdConsole;
protected:
    std::string Name;
    bool Enabled;
    bool LowerCaseArgs;
    bool AllowEmptyArgs;

    // Commands LRU cache
    unsigned LRUCount = 10; // Max latest commands to remember
    std::vector<std::string> CommandsCache;

public:
    xdConsoleCommand(std::string _name);
    virtual ~xdConsoleCommand();

    virtual void Execute(std::string args) = 0;

    std::string GetName();
    bool isEnabled();
    bool isLowerCaseArgs();
    bool isEmptyArgsAllowed();

    virtual std::string Status();
    virtual std::string Info();
    virtual std::string Syntax();
    void InvalidSyntax(std::string args);

    virtual void Save(filesystem::path&& p);

    virtual void AddCommandToCache(std::string&& cmd);
};
#pragma endregion Basic ConsoleCommand

#pragma region ConsoleCommand Boolean
class XDAY_API xdCC_Bool : public xdConsoleCommand
{
    using super = xdConsoleCommand;

public:
    xdCC_Bool(std::string _name, bool& _value);

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
class XDAY_API xdCC_Toggle : public xdConsoleCommand
{
    using super = xdConsoleCommand;

public:
    xdCC_Toggle(std::string _name, bool& _value);

    virtual void Execute(std::string args) override;

    virtual std::string Info() override;
    virtual std::string Status() override;

protected:
    bool& value;
};
#pragma endregion ConsoleCommand Toggle

#pragma region ConsoleCommand String
class XDAY_API xdCC_String : public xdConsoleCommand
{
    using super = xdConsoleCommand;
public:
    xdCC_String(std::string _name, std::string&& _value, unsigned _size);

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
class XDAY_API xdCC_Value : public xdConsoleCommand
{
    using super = xdConsoleCommand;
public:
    inline xdCC_Value(std::string _name, T& _value, T const _min, T const _max);
protected:
    T& value;
    T min;
    T max;
};
#pragma endregion ConsoleCommand Value Template

#pragma region ConsoleCommand Unsigned Integer
class XDAY_API xdCC_Unsigned : public xdCC_Value<unsigned>
{
    using super = xdCC_Value<unsigned>;

public:
    xdCC_Unsigned(std::string _name, unsigned& _value, unsigned const _min, unsigned const _max);

    virtual void Execute(std::string args) override;

    virtual std::string Info() override;
    virtual std::string Syntax() override;
    virtual std::string Status() override;
};
#pragma endregion ConsoleCommand Unsigned Integer

#pragma region ConsoleCommand Float
class XDAY_API xdCC_Float : public xdCC_Value<float>
{
    using super = xdCC_Value<float>;

public:
    xdCC_Float(std::string _name, float& _value, float const _min, float const _max);

    virtual void Execute(std::string args) override;

    virtual std::string Info() override;
    virtual std::string Syntax() override;
    virtual std::string Status() override;
};
#pragma endregion ConsoleCommand Float

#pragma region ConsoleCommand Config Loader
class XDAY_API xdCC_LoadConfig : public xdConsoleCommand
{
    using super = xdConsoleCommand;

public:
    xdCC_LoadConfig(std::string _name);
    void Execute(std::string args) override;
};
#pragma endregion ConsoleCommand Config Loader
