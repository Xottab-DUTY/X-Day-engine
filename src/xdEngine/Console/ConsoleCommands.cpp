#include "pch.hpp"

#include <fstream>

#include <GLFW/glfw3.h>

#include "xdEngine/xdEngine.hpp"
#include "Console.hpp"
#include "ConsoleCommand.hpp"
#include "ConsoleCommandBool.hpp"
#include "ConsoleCommandCall.hpp"
#include "ConsoleCommandString.hpp"
#include "ConsoleCommandValue.hpp"
#include "ConsoleCommands.hpp"
#include "ConsoleCommandsMacros.hpp"

namespace XDay
{
namespace Command
{
namespace Functions
{
void quit()
{
    glfwSetWindowShouldClose(Engine.windowMain, true);
}

void help(const std::string& args)
{
    if (!args.empty())
    {
        const auto cmd = Console.GetCommand(args);
        if (cmd)
            Info("{} = {}. Description: {}", cmd->GetName(), cmd->Status(), cmd->Help());
        else
            Info("Command not found.");
    }
    else
    {
        Info("Available commands:");
        for (auto cmd : Console.GetCommands())
            Info("{} : {}. Current value: {}. Syntax: {}", cmd->GetName(), cmd->Info(), cmd->Status(), cmd->Syntax());
    }
}

void config_save(const std::string& args)
{
    DebugMsg("Saving config file {}...", args.empty() ? Console.ConfigFile.string() : args);
    std::ofstream f(args.empty() ? Console.ConfigFile : args);

    for (auto cmd : Console.GetCommands())
        if (cmd->isSaveAllowed())
            f << cmd->Save() << std::endl;

    f.close();
    Info("Saved config file {}", args.empty() ? Console.ConfigFile.string() : args);
}

void config_load(const std::string& args)
{
    DebugMsg("Loading config file {}...", args.empty() ? Console.ConfigFile.string() : args);
    std::ifstream config_file(args.empty() ? Console.ConfigFile : args);

    if (config_file.is_open())
    {
        std::string line;
        while (getline(config_file, line))
            Console.Execute(line);
        Info("Loaded config file {}", args.empty() ? Console.ConfigFile.string() : args);
    }
    else
        Error("Failed to open config file {}", args.empty() ? Console.ConfigFile.string() : args);

    config_file.close();
}

void flush_log()
{
    FlushLog();
}

} // namespace Functions

int tint = 0;
float fint = 0.0;
double dint = 0.0;
Value<int> TestIntCC("test_int", test_description, tint, 0, 10);
Value<float> TestFloatCC("test_float", test_description, fint, 0, 10);
Value<double> TestDoubleCC("test_double", test_description, dint, 0, 10);

XDENGINE_API void RegisterConsoleCommands()
{
    CMDA(QuitCC);
    CMDA(ExitCC);
    CMDA(HelpCC);
    CMDA(FlushLogCC);
    CMDA(ConfigSaveCC);
    CMDA(ConfigLoadCC);
    CMDA(FullscreenCC);

    CMDA(TestIntCC);
    CMDA(TestFloatCC);
    CMDA(TestDoubleCC);

    CMD3(String, "test_string", test_description, "test!");
}

} // namespace Command
} // namespace XDay
