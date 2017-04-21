// 01.01.2017 Султан Xottab_DUTY Урамаев
// Чем стрелы коленом ловить, гораздо интереснее отстреливать свои ноги. Продолжим.
#include <iostream>
#include <thread>

#include <GLFW/glfw3.h>
#include <dynlib/Dynlib.hpp>

#include "xdCore.hpp"
#include "Console.hpp"
#include "ConsoleCommand.hpp"
#include "xdEngine.hpp"
#include "Debug/QuantMS.hpp"

void InitializeConsole()
{
    ConsoleCommands = new CC_Container;
    Console = new xdConsole;
    Console->Initialize();
}

void destroyConsole()
{
    ConsoleCommands->Execute("config_save");
    ConsoleCommands->Destroy();
    delete ConsoleCommands;
    Console->CloseLog();
    delete Console;
}

void HelpCmdArgs()
{
    Console->Log("\nAvailable parameters:\n"\
        "-name - Specifies AppName, default is \"X-Day Engine\" \n"\
        "-game - Specifies game module to be attached, default is \"xdGame\";\n"
        "-datapath - Specifies path of application data folder, default is \"*WorkingDirectory*/appdata\"\n"\
        "-respath - Specifies path of resources folder, default is \"*WorkingDirectory*/res\"\n"\
        "-mainconfig - Specifies path and name of main config file (path/name.extension), default is \"*DataPath*/main.config\" \n"\
        "-mainlog - Specifies path and name of main log file (path/name.extension), default is \"*DataPath*/main.log\"\n"\
        "-nolog - Completely disables engine log. May increase performance\n"\
        "-nologflush - Disables log flushing. Useless if -nolog defined\n");

    Console->Log("\nДоступные параметры:\n"\
        "-name - Задаёт AppName, по умолчанию: \"X-Day Engine\" \n"\
        "-game - Задаёт игровую библиотеку для подключения, по умолчанию: \"xdGame\";\n"
        "-datapath - Задаёт путь до папки с настройками, по умолчанию: \"*WorkingDirectory*/appdata\"\n"\
        "-respath - Задаёт путь до папки с ресурсами, по умолчанию: \"*WorkingDirectory*/res\"\n"\
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
        ConsoleCommands->Execute(input);
    }
    
}

void Startup()
{
    while (!glfwWindowShouldClose(Engine.window))
    {
        if (ConsoleCommands->GetBool("r_fullscreen"))
            glfwSetWindowMonitor(Engine.window, Engine.CurrentMonitor, 0, 0, Engine.CurrentMode->width, Engine.CurrentMode->height, Engine.CurrentMode->refreshRate);
        else
            glfwSetWindowMonitor(Engine.window, nullptr, 0, 0, Engine.CurrentMode->width-256, Engine.CurrentMode->height-256, Engine.CurrentMode->refreshRate);

        glfwPollEvents();
    }
}

int main(int argc, char* argv[])
{
    QuantMS();
#ifdef WINDOWS
    system("chcp 65001");
#endif
    Core.InitializeArguments(argc, argv);
    Core.Initialize("X-Day Engine");
    InitializeConsole();

    Console->Log(Core.GetGLFWVersionString());
    Console->Log(Core.GetBuildString());
    Console->Log("Core.Params: " + Core.Params);
    Console->Log("Девиз: Чем стрелы коленом ловить, гораздо интереснее отстреливать свои ноги. Продолжим.", false);
    Console->Log("Slogan: It's more interesting to shoot your feet, than catch arrows by your knee. Let's continue.");
    HelpCmdArgs();

    ConsoleCommands->ExecuteConfig(Console->ConfigFile.string());

    if (glfwInit())
        Console->Log("GLFW initialized.");
    else
        Console->Log("GLFW not initialized.");

    Engine.Initialize();
    Engine.xdCreateWindow();

    auto xdSoundModule = Dynlib::open(Core.GetModuleName("xdSound").c_str());
    if (xdSoundModule)
    {
        Console->Log("Module loaded successfully");
    }

    auto impFunc = (FunctionPointer)Dynlib::load(xdSoundModule, "funcToExport");
    if (impFunc)
        impFunc();
    else
        Console->Log("Failed to import function");

    if (Dynlib::close(xdSoundModule))
    {
        Console->Log("Module unloaded successfully");
    }

    std::thread WatchConsole(threadedConsole);
    WatchConsole.detach();
    Startup();

    glfwTerminate();
    Console->Log("GLFW terminated.");

    auto TotalTimer = QuantMS();
    ConsoleMsg("Total time: {} seconds", TotalTimer/1000000);
    destroyConsole();

    system("pause");
    return 0;
}
