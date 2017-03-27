#pragma once
#include <string>
#include "Console.hpp"
#include <filesystem>
namespace filesystem = std::experimental::filesystem::v1;

/*
class XDENGINE_API xdConsoleCommand
{
public:
    friend class xdConsole;
private:
    std::string Name;
    bool Enabled;
    bool LowerCaseArgs;
    bool AllowEmptyArgs;

    // Commands LRU cache
    unsigned LRUCount = 10; // Max latest commands to remember
    std::vector<std::string> cmdLRUcache;

public:
    xdConsoleCommand(std::string&& name);
    ~xdConsoleCommand();
    std::string GetName() { return Name; }
    void InvalidSyntax();
    virtual void Execute(std::string&& args);
    virtual std::string Status();
    virtual void Info(std::string&& info);
    virtual void Save(filesystem::path&& p);

    virtual void AddCommandToLRUCache(std::string&& cmd);
};*/
