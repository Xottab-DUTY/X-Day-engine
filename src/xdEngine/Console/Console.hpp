#pragma once

#include <filesystem>
namespace filesystem = std::experimental::filesystem;

#include "ConsoleCommand.hpp"
#include "ConsoleCommands.hpp"

namespace XDay
{
class XDENGINE_API console
{
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
    void ExecuteConfig() const { ExecuteConfig(ConfigFile); }
    void ExecuteConfig(const filesystem::path& path) const;

    void AddCommandToCache(const std::string& cmd);
};
} // namespace XDay

extern XDENGINE_API XDay::console Console;
