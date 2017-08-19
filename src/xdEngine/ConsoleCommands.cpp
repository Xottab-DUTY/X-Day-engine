#include <sstream>
#include <fstream>
#include <locale>

#include <GLFW/glfw3.h>

#include "Debug/Log.hpp"
#include "xdCore.hpp"
#include "xdEngine.hpp"
#include "XMLResource.hpp"
#include "ConsoleCommand.hpp"
#include "ConsoleCommands.hpp"

using namespace XDay;

void CC_Container::ExecuteConfig(const filesystem::path& filename) const
{
    Execute(&ConfigLoadCC, filename.string());
}

namespace cc_functions
{
    void CC_ConfigLoad(const std::string& args)
    {
        Msg("Loading config file {}...", args.empty() ? Console->ConfigFile.string() : args);
        std::ifstream config_file(args.empty() ? Console->ConfigFile : args);
        std::string line;

        if (config_file.is_open())
        {
            while (std::getline(config_file, line))
                ConsoleCommands->Execute(line);
            Msg("Loaded config file {}", args.empty() ? Console->ConfigFile.string() : args);
        }
        else
            Msg("Failed to open config file {}", args.empty() ? Console->ConfigFile.string() : args);

        config_file.close();
    }

    void CC_ConfigSave(const std::string& args)
    {
        Msg("Saving config file {}...", args.empty() ? Console->ConfigFile.string() : args);
        std::ofstream f(args.empty() ? Console->ConfigFile : args);
        for (auto str : ConsoleCommands->CommandsContainer)
        {
            auto CommandToSave = str.second;
            if (CommandToSave->isSavingAllowed())
                f << CommandToSave->Save() << std::endl;
        }
        f.close();
        Msg("Saved config file {}", args.empty() ? Console->ConfigFile.string() : args);
    }

    void CC_Help(const std::string& args)
    {
        ConsoleCommand* CommandToHelp;
        if (!args.empty())
        {
            CommandToHelp = ConsoleCommands->GetCommand(args);
            if (CommandToHelp)
                Msg("{} : {}. Current value: {}. Syntax: {}", CommandToHelp->GetName(), CommandToHelp->Info(), CommandToHelp->Status(), CommandToHelp->Syntax())
            else
                Log("Command not found.");
        }
        else
        {
            Log("Available commands:");
            for (auto str : ConsoleCommands->CommandsContainer)
            {
                CommandToHelp = str.second;
                Msg("{} : {}. Current value: {}. Syntax: {}", CommandToHelp->GetName(), CommandToHelp->Info(), CommandToHelp->Status(), CommandToHelp->Syntax());
            }
        }
    }

    void CC_Exit(const std::string& args)
    {
        glfwSetWindowShouldClose(Engine.window, true);
    }

    void CC_FlushLog(const std::string& args)
    {
        FlushLog();
    }

    void CC_SystemCommand(const std::string& args)
    {
        system(args.c_str());
    }
}

void CC_FCallTest(const std::string& args)
{
    Msg("Function call test {}", args);
}

void CC_XMLTest(const std::string& args)
{
    Log("XML test");

    Log("XML tested");
}

int int_test = 1;
float float_test = 1.0;
double double_test = 1.0;
bool toggle_test = false;
std::string string_test = "test";

bool GlobalFullscreen = false;

void RegisterConsoleCommands()
{
    /*
    A bit of help:

    CMDA – Adds already declared function

    CC_FunctionCall – CMD3(CC_FunctionCall, command_name_in_this_file, "command_name_in_console", function_to_call, is_empty_args_allowed);

    CC_Bool         – CMD2(CC_Bool,         command_name_in_this_file, "command_name_in_console", variable_to_change);

    CC_Toggle       – CMD2(CC_Toggle,       command_name_in_this_file, "command_name_in_console", variable_to_change);

    CC_Integer      – CMD4(CC_Integer,      command_name_in_this_file, "command_name_in_console", variable_to_change, minimum_value, maximum_value);

    CC_Float        – CMD4(CC_Float,        command_name_in_this_file, "command_name_in_console", variable_to_change, minimum_value, maximum_value);

    CC_Double       – CMD4(CC_Double,       command_name_in_this_file, "command_name_in_console", variable_to_change, minimum_value, maximum_value);

    CC_String       – CMD3(CC_String,       command_name_in_this_file, "command_name_in_console", variable_to_change, max_string_size);
    */
    CMDA(ExitCC);
    CMDA(QuitCC);
    CMDA(HelpCC);
    CMDA(SystemCommandCC);

    CMDA(FlushLogCC);

    CMDA(ConfigLoadCC);
    CMDA(ConfigSaveCC);

    CMDA(FullscreenCC);

    CMD4(CC_FunctionCall, XMLTestCC, "test_xml", CC_XMLTest, true, Core.isGlobalDebug());
    CMD4(CC_FunctionCall, FCallTestCC, "test_fcall", CC_FCallTest, true, Core.isGlobalDebug());
    CMD3(CC_Toggle, toggle_testCC, "test_toggle", toggle_test, Core.isGlobalDebug());
    CMD5(CC_Integer, int_testCC, "test_int", int_test, 1, 10, Core.isGlobalDebug());
    CMD5(CC_Float, float_testCC, "test_float", float_test, 1.0, 10.0, Core.isGlobalDebug());
    CMD5(CC_Double, double_testCC, "test_double", double_test, 1.0, 10.0, Core.isGlobalDebug());
    CMD4(CC_String, string_testCC, "test_string", string_test, 512, Core.isGlobalDebug());
}
