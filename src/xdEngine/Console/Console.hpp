#pragma once

#include <filesystem>
namespace filesystem = std::experimental::filesystem;

#include "Common/import_export_macros.hpp"

#include "ConsoleCommand.hpp"
#include "ConsoleCommands.hpp"

namespace XDay
{
class XDAY_API console
{
    std::vector<Command::command*> commands;

public:
    bool isVisible = false;
    filesystem::path ConfigFile;

    // Commands LRU cache
    unsigned LRUCount = 10; // Max latest commands to remember
    std::vector<std::string> CommandsCache;

    void Initialize();

    void Show();
    void Hide();

    void Execute(const std::string& cmd);
    static void Execute(Command::command* cmd);
    static void Execute(Command::command* cmd, std::string&& args);
    void ExecuteConfig() const { ExecuteConfig(ConfigFile); }
    void ExecuteConfig(const filesystem::path& path) const;

    static void ExecuteBool(Command::Bool* cmd, bool value);

    Command::command* GetCommand(const std::string& cmd) const;

    bool GetBool(const std::string& cmd) const;
    static bool GetBool(Command::Bool* cmd);

    void AddCommand(Command::command* cmd);
    void RemoveCommand(Command::command* cmd);

    void AddCommandToCache(const std::string& cmd);

    std::vector<Command::command*> GetCommands() const { return commands; }
};
} // namespace XDay

extern XDAY_API XDay::console Console;
