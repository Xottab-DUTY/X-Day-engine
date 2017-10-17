// 01.01.2017 Султан Xottab_DUTY Урамаев
// Чем стрелы коленом ловить, гораздо интереснее отстреливать свои ноги. Продолжим.
#include "pch.hpp"

#include <iostream>
#include <thread>

#include <GLFW/glfw3.h>
#include "dynlib/Dynlib.hpp"

#include "Common/Platform.hpp"
#include "xdCore/Log.hpp"
#include "xdCore/xdCore.hpp"
#include "ConsoleCommand.hpp"
#include "ConsoleCommands.hpp"
#include "xdEngine.hpp"

void InitializeConsole()
{
    ConsoleCommands = new XDay::CC_Container;
    Console = new XDay::xdConsole;
    Console->Initialize();
}

void destroyConsole()
{
    ConsoleCommands->Execute(&ConfigSaveCC);
    ConsoleCommands->Execute(&FlushLogCC);
    ConsoleCommands->Destroy();
    delete ConsoleCommands;
    delete Console;
}

void watch_console()
{
#ifdef DEBUG
    const bool allowed = true;
#else
    const bool allowed = Core.FindParam(XDay::eParamDontHideSystemConsole);
#endif

    while (allowed && !glfwWindowShouldClose(Engine.windowMain))
    {
        std::string input;
        std::getline(std::cin, input);
        if (Console && ConsoleCommands && !glfwWindowShouldClose(Engine.windowMain))
            ConsoleCommands->Execute(input);
        else
            break;
    }
}

void AttachRenderer()
{
    const auto handle = Dynlib::open(Core.GetModuleName(XDay::eRendererModule).c_str());

    const auto func = (FunctionPointer)Dynlib::load(handle, "InitializeRenderer");
    if (func)
        func();
    else
        Error("Cannot attach function InitializeRenderer from {}", Core.GetModuleName(XDay::eRendererModule));
}

int main(int argc, char* argv[])
{
    system("chcp 65001");

    Core.InitializeArguments(argc, argv);

#ifdef WINDOWS
    if (!Core.FindParam(XDay::eParamDontHideSystemConsole))
        FreeConsole();
#endif

    Core.Initialize();
    InitializeConsole();

    DebugMsg(Core.GetGLFWVersionString());
    Info(Core.GetBuildString());
    Info("Core.Params: " + Core.ParamsString);
    Info("Девиз: Чем стрелы коленом ловить, гораздо интереснее отстреливать свои ноги. Продолжим.");
    Info("Slogan: It's more interesting to shoot your feet, than catch arrows by your knee. Let's continue.");
    Core.GetParamsHelp();

    ConsoleCommands->ExecuteConfig(Console->ConfigFile);
    ErrorIf(!glfwInit(), "GLFW not initialized.");

    AttachRenderer();

    Engine.Initialize();
    Engine.createMainWindow();

    std::thread WatchConsole(watch_console);

    Engine.InitRender();

    Engine.mainLoop();

    WatchConsole.~thread();

    glfwTerminate();

    destroyConsole();

    return 0;
}
