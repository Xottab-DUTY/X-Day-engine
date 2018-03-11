#include "pch.hpp"

#include <sstream>

#include "xdCore/xdCore.hpp"
#include "xdCore/CommandLine/Keys.hpp"
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
        : ConfigFile = Core.DataPath.string() + "main.config";

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

void console::Execute(Command::command* cmd)
{
    if (!cmd)
        throw "Command execution failed!";

    if (cmd->isEnabled())
    {
        if (cmd->isEmptyArgsAllowed())
            cmd->Execute();
        else
            Log::Info("{} {}", cmd->GetName(), cmd->Status());
    }
    else
        Log::Info("Command is disabled: {}", cmd->GetName());
}

void console::Execute(Command::command* cmd, std::string&& args)
{
    if (!cmd)
        throw "Command execution failed!";

    if (cmd->isEnabled())
    {
        if (args.empty())
        {
            if (cmd->isEmptyArgsAllowed())
                cmd->Execute();
            else
                Log::Info("{} {}", cmd->GetName(), cmd->Status());
        }
        else
        {
            if (cmd->isLowerCaseArgs())
            {
                const std::locale loc;
                for (auto&& elem:args)
                    elem = tolower(elem, loc);
            }
            cmd->Execute(args);
        }
    }
    else
        Log::Info("Command is disabled: {}", cmd->GetName());
}

void console::ExecuteConfig(const filesystem::path& path) const
{
    Execute(&Command::ConfigLoadCC, path.string());
}

void console::ExecuteBool(Command::Bool* cmd, bool value)
{
    if (cmd)
    {
        if (cmd->isEnabled())
            cmd->Execute(value);
        else
            Log::Info("Command is disabled: {}", cmd->GetName());
    }
    else
        Log::Error("Unknown command.");
}

Command::command* console::GetCommand(const std::string& cmd) const
{
    for (auto i : commands)
        if (i->GetName() == cmd)
            return i;
    return nullptr;
}

bool console::GetBool(const std::string& cmd) const
{
    const auto command = static_cast<Command::Bool*>(GetCommand(cmd));
    if (command)
        return command->GetValue();
    return false;
}

bool console::GetBool(Command::Bool* cmd)
{
    if (cmd)
        return cmd->GetValue();
    Log::Error("console::GetBool(): command is nullptr");
    return false;
}

void console::AddCommand(Command::command* cmd)
{
    for (auto i : commands)
    {
        if (i->GetName() == cmd->GetName())
        {
            Log::Warning("console::AddCommand({}):: dublicate found.", cmd->GetName());
            return;
        }
    }

    commands.emplace_back(cmd);
}

void console::RemoveCommand(Command::command* cmd)
{
    commands.erase(std::remove(commands.begin(), commands.end(), cmd), commands.end());
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
