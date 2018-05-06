#include "pch.hpp"

#include <fstream>

#include <GLFW/glfw3.h>

#include "xdCore/Console/ConsoleCommand.hpp"
#include "xdCore/Console/ConsoleCommands.hpp"
#include "xdCore/Console/ConsoleCommandsMacros.hpp"
#include "ConsoleCommands.hpp"
#include "Console.hpp"

#include "xdEngine/xdEngine.hpp"

namespace XDay::Console
{
namespace Calls
{
void Quit()
{
    glfwSetWindowShouldClose(Engine.windowMain, true);
}

void ConfigSave(stringc&& args)
{
    Log::Debug("Saving config file {}...", args.empty() ? Console.ConfigFile.string() : args);
    std::ofstream f(args.empty() ? Console.ConfigFile : args);

    for (auto cmd : Commands::Get())
        if (cmd->SaveAllowed())
            f << cmd->Save() << std::endl;

    f.close();
    Log::Info("Saved config file {}", args.empty() ? Console.ConfigFile.string() : args);
}

void ConfigLoad(stringc&& args)
{
    std::ifstream config_file(args.empty() ? Console.ConfigFile : args);

    if (config_file.is_open())
    {
        string line;
        while (getline(config_file, line))
            Commands::Execute(line.c_str());
        Log::Info("Loaded config file {}", args.empty() ? Console.ConfigFile.string() : args);
    }
    else
        Log::Error("Failed to open config file {}", args.empty() ? Console.ConfigFile.string() : args);

    config_file.close();
}
} // namespace Calls

int tint = 0;
float fint = 0.0;
double dint = 0.0;
string sint;
Command<int> TestInt("test_int", testDescription, tint, 0, 10);
Command<float> TestFloat("test_float", testDescription, fint, 0, 10);
Command<double> TestDouble("test_double", testDescription, dint, 0, 10);
Command<string> TestString("test_string", testDescription, sint, 0);

void RegisterEngineCommands()
{
    CMDA(Quit);
    CMDA(Exit);
    CMDA(ConfigSave);
    CMDA(ConfigLoad);
    CMDA(Fullscreen);

    CMDA(TestInt);
    CMDA(TestFloat);
    CMDA(TestDouble);
    CMDA(TestString);
}

} // namespace XDay::Console
