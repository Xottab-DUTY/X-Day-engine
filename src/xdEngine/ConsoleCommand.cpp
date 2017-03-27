#include <fstream>

#include "ConsoleCommand.hpp"

/*
xdConsoleCommand::xdConsoleCommand(std::string&& name) : Name(name), Enabled(true), LowerCaseArgs(true), AllowEmptyArgs(false)
{
    cmdLRUcache.reserve(LRUCount + 1);
    cmdLRUcache.erase(cmdLRUcache.begin(), cmdLRUcache.end());
}

void xdConsoleCommand::Save(filesystem::path&& p)
{
    std::ofstream f;
    f.open(p);
    f << Status();
    f.close();
}

void xdConsoleCommand::AddCommandToLRUCache(std::string&& cmd)
{
    if (cmd.size() == 0 || AllowEmptyArgs)
    {
        return;
    }
    bool isDublicate = std::find(cmdLRUcache.begin(), cmdLRUcache.end(), cmd) != cmdLRUcache.end();
    if (!isDublicate)
    {
        cmdLRUcache.push_back(cmd);
        if (cmdLRUcache.size() > LRUCount)
        {
            cmdLRUcache.erase(cmdLRUcache.begin());
        }
    }
}*/
