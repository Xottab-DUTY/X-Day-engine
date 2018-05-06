#include "pch.hpp"

#include "xdCore/CommandLine/Keys.hpp"
#include "xdCore/Filesystem.hpp"
#include "Console.hpp"
#include "ConsoleCommands.hpp"

XDENGINE_API XDay::console Console;

namespace XDay
{
void console::Initialize()
{
    CommandLine::KeyMainConfig.IsSet()
        ? ConfigFile = CommandLine::KeyMainConfig.StringValue()
        : ConfigFile = FS.DataPath.string() + "main.config";

    CommandsCache.reserve(LRUCount + 1);
    CommandsCache.erase(CommandsCache.begin(), CommandsCache.end());

    Console::RegisterEngineCommands();
}

void console::Show()
{
    if (isVisible)
        return;
    isVisible = true;
}

void console::Hide()
{
    if (!isVisible)
        return;
    isVisible = false;
}

void console::ExecuteConfig(const filesystem::path& path) const
{
    Console::ConfigLoad.Execute(path.string());
}

void console::AddCommandToCache(const std::string& cmd)
{
    if (cmd.empty())
        return;

    // Don't add this if this is a duplicate
    if (std::find(CommandsCache.begin(), CommandsCache.end(), cmd) == CommandsCache.end())
    {
        CommandsCache.push_back(cmd);
        if (CommandsCache.size() > LRUCount)
            CommandsCache.erase(CommandsCache.begin());
    }
}
} // namespace XDay
