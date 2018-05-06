#include "pch.hpp"

#include <sstream>

#include "xdCore/Core.hpp"
#include "xdCore/CommandLine/Keys.hpp"
#include "xdCore/Filesystem.hpp"
#include "Console.hpp"
#include "ConsoleCommand.hpp"
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

    Command::RegisterConsoleCommands();
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

void console::Execute(const std::string& cmd)
{
    std::string cmd_str, cmd_val;

    std::istringstream buffer(cmd);
    buffer >> cmd_str;
    for (std::string i; buffer >> i;)
    {
        cmd_val += i + " ";
    }
    if (!cmd_val.empty())
        cmd_val.pop_back(); // remove the last " "

    const auto command = GetCommand(cmd_str);

    if (!command)
    {
        Log::Info("Unknown command: {}", cmd_str);
        return;
    }

    if (command->isEnabled())
    {
        if (cmd_val.empty())
        {
            if (command->isEmptyArgsAllowed())
                command->Execute(cmd_val);
            else
                Log::Info("{} {}", command->GetName(), command->Status());
        }
        else
            command->Execute(cmd_val);
    }
    else
    {
        if (command->isLowerCaseArgs())
        {
            const std::locale loc;
            for (auto&& elem : cmd_val)
                elem = tolower(elem, loc);
        }
        Log::Info("Command is disabled: {}", cmd_str);
    }
    AddCommandToCache(cmd);
}
void console::ExecuteConfig(const filesystem::path& path) const
{
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
