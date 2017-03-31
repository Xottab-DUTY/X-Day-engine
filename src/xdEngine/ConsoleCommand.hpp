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
    std::map<const char*, xdConsoleCommand*> CommandsContainer;

    void AddCommand(xdConsoleCommand* cc);
    void RemoveCommand(xdConsoleCommand* cc);
    bool Execute(const char* args);
};
#pragma endregion ConsoleCommand Container

XDAY_API extern xdCC_Container* ConsoleCommands;

#pragma region ConsoleCommand Adding Macros
#define CMD0(cls) { static cls x##cls(); ConsoleCommands->AddCommand(&x##cls); }
#define CMD1(cls,p1) { static cls x##cls(p1); ConsoleCommands->AddCommand(&x##cls); }
#define CMD2(cls,p1,p2) { static cls x##cls(p1,p2); ConsoleCommands->AddCommand(&x##cls); }
#define CMD3(cls,p1,p2,p3) { static cls x##cls(p1,p2,p3); ConsoleCommands->AddCommand(&x##cls); }
#define CMD4(cls,p1,p2,p3,p4) { static cls x##cls(p1,p2,p3,p4); ConsoleCommands->AddCommand(&x##cls); }
#pragma endregion ConsoleCommand Adding Macros

#pragma region Basic ConsoleCommand
class XDAY_API xdConsoleCommand
{
public:
    friend class xdConsole;
private:
    const char* Name;
    bool Enabled;
    bool LowerCaseArgs;
    bool AllowEmptyArgs;

    // Commands LRU cache
    unsigned LRUCount = 10; // Max latest commands to remember
    std::vector<std::string> CommandsCache;

public:
    xdConsoleCommand(const char* _name);
    virtual ~xdConsoleCommand();

    virtual void Execute(const char* args) = 0;

    const char* GetName();
    virtual const char* Status();
    virtual const char* Info();
    virtual const char* Syntax();
    void InvalidSyntax(const char* args);

    virtual void Save(filesystem::path&& p);

    virtual void AddCommandToCache(std::string&& cmd);
};
#pragma endregion Basic ConsoleCommand

#pragma region ConsoleCommand Boolean
class XDAY_API xdCC_Bool : public xdConsoleCommand
{
    using super = xdConsoleCommand;

public:
    xdCC_Bool(const char* _name, bool& _value);

    virtual void Execute(const char* args) override;

    virtual const char* Info() override;
    virtual const char* Syntax() override;
    virtual const char* Status() override;

protected:
    bool& value;
};
#pragma endregion ConsoleCommand Boolean

#pragma region ConsoleCommand String
class XDAY_API xdCC_String : public xdConsoleCommand
{
    using super = xdConsoleCommand;
public:
    xdCC_String(const char* _name, std::string&& _value, unsigned _size);

    virtual void Execute(const char* args) override;

    virtual const char* Info() override;
    virtual const char* Syntax() override;
    virtual const char* Status() override;

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
    inline xdCC_Value(const char* _name, T& _value, T const _min, T const _max);
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
    xdCC_Unsigned(const char* _name, unsigned& _value, unsigned const _min, unsigned const _max);

    virtual void Execute(const char* args) override;

    virtual const char* Info() override;
    virtual const char* Syntax() override;
    virtual const char* Status() override;
};
#pragma endregion ConsoleCommand Unsigned Integer

#pragma region ConsoleCommand Float
class XDAY_API xdCC_Float : public xdCC_Value<float>
{
    using super = xdCC_Value<float>;

public:
    xdCC_Float(const char* _name, float& _value, float const _min, float const _max);

    virtual void Execute(const char* args) override;

    virtual const char* Info() override;
    virtual const char* Syntax() override;
    virtual const char* Status() override;
};
#pragma endregion ConsoleCommand Float