#pragma once
#include <string>
#include <map>
#include <filesystem>
namespace filesystem = std::experimental::filesystem::v1;

#include "xdEngine.hpp"

class XDENGINE_API xdConsoleCommand;

class XDENGINE_API xdCC_Container
{
public:
    std::map<const char*, xdConsoleCommand*> CommandsContainer;

    void AddCommand(xdConsoleCommand* cc);
    void RemoveCommand(xdConsoleCommand* cc);
    bool Execute(const char* args);
};

XDENGINE_API extern xdCC_Container* ConsoleCommands;

class XDENGINE_API xdConsoleCommand
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
    const char* GetName() { return Name; }
    void InvalidSyntax();
    
    //virtual const char* Status();
    //virtual void Info(const char* info);
    virtual void Save(filesystem::path&& p);

    virtual void AddCommandToCache(std::string&& cmd);
};

class XDENGINE_API xdCC_Bool : public xdConsoleCommand
{
    using super = xdConsoleCommand;

public:
    xdCC_Bool(const char* _name, bool& _value);

protected:
    bool& value;
};

template<class T>
class XDENGINE_API xdCC_Value : public xdConsoleCommand
{
    using super = xdConsoleCommand;
public:
    inline xdCC_Value(const char* _name, T& _value, T const _min, T const _max);
protected:
    T& value;
    T min;
    T max;
};

class XDENGINE_API xdCC_Unsigned : public xdCC_Value<unsigned>
{
    using super = xdCC_Value<unsigned>;

public:
    xdCC_Unsigned(const char* _name, unsigned& _value, unsigned const _min, unsigned const _max);
};