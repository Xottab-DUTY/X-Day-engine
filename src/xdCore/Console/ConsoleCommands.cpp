#include "pch.hpp"

#include "ConsoleCommand.hpp"
#include "ConsoleCommands.hpp"
#include "ConsoleCommandsMacros.hpp"

namespace XDay::Console
{
Commands Commands::instance;

namespace Calls
{
void Help(stringc&& args)
{
    const auto cmd = Commands::Get(std::move(args));
    if (cmd)
        Log::Info("{}<{}> = [{}] ({}) {{}}", cmd->Name(), cmd->Type(), cmd->Status(), cmd->Help(), cmd->Syntax());
    else
        Log::Error("Unknown console command: [{}]", args);
}

void Help()
{
    Log::Info("Available commands:");
    for (const auto& cmd : Commands::Get())
        Log::Info("{}<{}> = [{}] ({})", cmd->Name(), cmd->Type(), cmd->Status(), cmd->Help());
}
} // namespace Calls

Commands::Commands()
{
    CMDA(Terminate);
    CMDA(FlushLog);
    CMDA(Help);
}


void Commands::Register(ICommand* command) noexcept
{
    for (const auto& i : instance.commands)
    {
        if (i->Name() == command->Name())
        {
            Log::Warning("Commands::Register({}):: dublicate found.", command->Name());
            return;
        }
    }

    instance.commands.emplace_back(command);
}

void Commands::Unregister(ICommand* command) noexcept
{
    if (!Registered(command))
        return;

    auto& commands = instance.commands;
    commands.erase(std::remove(commands.begin(), commands.end(), command), commands.end());
}

bool Commands::Registered(stringc&& needed)
{
    if (Get(std::move(needed)) != nullptr)
        return true;
    return false;
}

bool Commands::Registered(ICommand* needed)
{
    for (auto& command : instance.commands)
        if (command == needed)
            return true;
    return false;
}

ICommand* Commands::Get(stringc&& needed)
{
    for (auto& command : instance.commands)
        if (command->Name() == needed)
            return command;

    Log::Error("Unknown console command: [{}]", needed);
    return nullptr;
}

template <typename Type>
Type Commands::GetValue(stringc&& command)
{
    return static_cast<CommandBase<Type>>(Get(std::move(command))).Value();
}

bool Commands::Execute(stringc&& command)
{
    return Execute(Get(std::move(command)));
}

bool Commands::Execute(stringc&& command, string&& args)
{
    return Execute(Get(std::move(command)), std::move(args));
}

bool Commands::Execute(ICommand* command)
{
    if (command == nullptr)
        return false;

    if (command->Enabled())
    {
        if (command->EmptyArgumentsHandled())
        {
            command->Execute();
            return true;
        }
        Log::Info("{} {}", command->Name(), command->Status());
    }

    return false;
}

bool Commands::Execute(ICommand* command, string&& args)
{
    if (command == nullptr)
        return false;

    if (command->Enabled())
    {
        if (args.empty())
        {
            if (command->EmptyArgumentsHandled())
            {
                command->Execute();
                return true;
            }
            Log::Info("{} {}", command->Name(), command->Status());
        }
        else
        {
            if (command->LowerCaseArguments())
            {
                const std::locale loc;
                for (auto&& elem : args)
                    elem = tolower(elem, loc);
            }
            command->Execute(std::move(args));
            return true;
        }
    }

    return false;
}
} // namespace XDay::Console
