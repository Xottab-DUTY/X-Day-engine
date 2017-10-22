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
#include "xdEngine/Console/Console.hpp"
#include "xdEngine/xdEngine.hpp"

void watch_console()
{
    const bool allowed = Core.FindParam(XDay::eParamDontHideSystemConsole);

    while (allowed && !glfwWindowShouldClose(Engine.windowMain))
    {
        std::string input;
        std::getline(std::cin, input);
        if (!glfwWindowShouldClose(Engine.windowMain))
            Console.Execute(input);
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
    //Dynlib::close(handle);
}

int main(int argc, char* argv[])
{
    Core.InitializeArguments(argc, argv);

#ifdef WINDOWS
    SetConsoleCP(65001);
    SetConsoleOutputCP(65001);
    if (!Core.FindParam(XDay::eParamDontHideSystemConsole))
        FreeConsole();
#endif

    Core.Initialize();
    Console.Initialize();

    DebugMsg(Core.GetGLFWVersionString());
    Info(Core.GetBuildString());
    Info("Core.Params: " + Core.ParamsString);
    Info("Девиз: Чем стрелы коленом ловить, гораздо интереснее отстреливать свои ноги. Продолжим.");
    Info("Slogan: It's more interesting to shoot your feet, than catch arrows by your knee. Let's continue.");
    Core.GetParamsHelp();

    Console.ExecuteConfig();
    ErrorIf(!glfwInit(), "GLFW not initialized.");

    AttachRenderer();

    Engine.Initialize();
    Engine.createMainWindow();

    std::thread WatchConsole(watch_console);

    Engine.InitRender();

    Engine.mainLoop();

    WatchConsole.~thread();

    glfwTerminate();

    return 0;
}
