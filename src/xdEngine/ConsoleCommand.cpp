#include <fstream>

#include "ConsoleCommand.hpp"

void xdCC_Container::AddCommand(xdConsoleCommand* cmdName)
{
    CommandsContainer[cmdName->GetName()] = cmdName;
}

void xdCC_Container::RemoveCommand(xdConsoleCommand* cmdName)
{
    
}

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

void xdConsoleCommand::Save(filesystem::path&& p)
{
    std::ofstream f;
    f.open(p);
    //f << Status();
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

xdCC_Bool::xdCC_Bool(const char* _name, bool& _value) : super(_name), value(_value) {}

template<class T>
inline xdCC_Value<T>::xdCC_Value(const char* _name, T& _value, T const _min, T const _max)
    : super(_name), value(_value), min(_min), max(_max) {}

xdCC_Unsigned::xdCC_Unsigned(const char* _name, unsigned& _value, unsigned const _min, unsigned const _max)
    : super(_name, _value, _min, _max) {}