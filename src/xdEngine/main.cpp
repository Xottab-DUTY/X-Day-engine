// 01.01.2017 Султан Xottab_DUTY Урамаев
// Чем стрелы коленом ловить, гораздо интереснее отстреливать свои ноги. Продолжим.
#include <iostream>
#include <thread>

#include <GLFW/glfw3.h>
#include <dynlib/Dynlib.hpp>

#include "Common/Platform.hpp"
#include "Debug/Log.hpp"
#include "xdCore.hpp"
#include "ConsoleCommand.hpp"
#include "xdEngine.hpp"
#include "xdRender/xdRender.hpp"

void InitializeConsole()
{
    ConsoleCommands = new CC_Container;
    Console = new xdConsole;
    Console->Initialize();
}

void destroyConsole()
{
    ConsoleCommands->Execute(&ConfigSaveCC);
    ConsoleCommands->Destroy();
    delete ConsoleCommands;
    delete Console;
}

void HelpCmdArgs()
{
    Log("\nAvailable parameters:\n"\
        "--p_name - Specifies AppName, default is \"X-Day Engine\" \n"\
        "--p_game - Specifies game module to be attached, default is \"xdGame\";\n"
        "--p_datapath - Specifies path of application data folder, default is \"*WorkingDirectory*/appdata\"\n"\
        "--p_respath - Specifies path of resources folder, default is \"*WorkingDirectory*/resources\"\n"\
        "--p_mainconfig - Specifies path and name of main config file (path/name.extension), default is \"*DataPath*/main.config\" \n"\
        "--p_mainlog - Specifies path and name of main log file (path/name.extension), default is \"*DataPath*/main.log\"\n"\
        "--p_nolog - Completely disables engine log. May increase performance\n"\
        "--p_nologflush - Disables log flushing. Useless if -nolog defined\n");

    Log("\nДоступные параметры:\n"\
        "-name - Задаёт AppName, по умолчанию: \"X-Day Engine\" \n"\
        "-game - Задаёт игровую библиотеку для подключения, по умолчанию: \"xdGame\";\n"
        "-datapath - Задаёт путь до папки с настройками, по умолчанию: \"*WorkingDirectory*/appdata\"\n"\
        "-respath - Задаёт путь до папки с ресурсами, по умолчанию: \"*WorkingDirectory*/resources\"\n"\
        "-mainconfig - Задаёт путь и имя главного файла настроек (путь/имя.расширение), по умолчанию: \"*DataPath*/main.config\" \n"\
        "-mainlog - Задаёт путь и имя главного лог файла (путь/имя.расширение), по умолчанию: \"*DataPath*/main.log\"\n"\
        "-nolog - Полностью выключает лог движка. Может повысить производительность\n"\
        "-nologflush - Выключает сброс лога в файл. Не имеет смысла если задан -nolog\n", false);
}

void threadedConsole()
{
    while (!glfwWindowShouldClose(Engine.window))
    {
        std::string input;
        std::getline(std::cin, input);
        if (Console && ConsoleCommands && !glfwWindowShouldClose(Engine.window))
            ConsoleCommands->Execute(input);
        else
            break;
    }
    
}

void Startup()
{
    while (!glfwWindowShouldClose(Engine.window))
    {
        glfwPollEvents();
    }
}

int main(int argc, char* argv[])
{
#ifdef WINDOWS
    //system("chcp 65001");
#endif
    //setlocale(LC_ALL, "");
    InitLogger();

    Core.InitializeArguments(argc, argv);
    Core.Initialize("X-Day Engine");
    InitializeConsole();

    Log(Core.GetGLFWVersionString());
    Log(Core.GetBuildString());
    Log("Core.Params: " + Core.ParamsString);
    Log("Девиз: Чем стрелы коленом ловить, гораздо интереснее отстреливать свои ноги. Продолжим.", false);
    Log("Slogan: It's more interesting to shoot your feet, than catch arrows by your knee. Let's continue.");
    HelpCmdArgs();

    ConsoleCommands->ExecuteConfig(Console->ConfigFile.string());

    if (glfwInit())
        Log("GLFW initialized.");
    else
        Log("GLFW not initialized.");

    Engine.Initialize();
    Engine.xdCreateWindow();
    Render.Initialize();

    /*auto xdSoundModule = Dynlib::open(Core.GetModuleName("xdSound").c_str());
    if (xdSoundModule)
        Log("Module loaded successfully");
    else
        Log("Failed to load module");

    auto impFunc = (FunctionPointer)Dynlib::load(xdSoundModule, "funcToExport");
    if (impFunc)
        impFunc();
    else
        Log("Failed to import function");

    if (Dynlib::close(xdSoundModule))
        Log("Module unloaded successfully");*/

    std::thread WatchConsole(threadedConsole);
    WatchConsole.detach();
    Startup();
    WatchConsole.~thread();

    glfwTerminate();
    Log("GLFW terminated.");

    destroyConsole();

    return 0;
}
