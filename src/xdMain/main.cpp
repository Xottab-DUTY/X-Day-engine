// 01.01.2017 Султан Xottab_DUTY Урамаев
// Чем стрелы коленом ловить, гораздо интереснее отстреливать свои ноги. Продолжим.
#include "pch.hpp"

#include <iostream>
#include <thread>

#include <GLFW/glfw3.h>

#include "xdCore/xdCore.hpp"
#include "xdEngine/Console/Console.hpp"
#include "xdEngine/xdEngine.hpp"
#include "xdCore/ModuleManager.hpp"

using namespace XDay;

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
    ModuleManager::LoadModule(XDay::eRendererModule);

    using pFunc = void(*)();
    const auto func = static_cast<pFunc>(ModuleManager::GetProcFromModule(XDay::eRendererModule, "InitializeRenderer"));
    if (func)
        func();
    else
        Log::Error("Cannot attach function InitializeRenderer from {}", ModuleManager::GetModuleName(XDay::eRendererModule));
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

    Log::Debug(Core.GetGLFWVersionString());
    Log::Info(Core.GetBuildString());
    Log::Info("Core.Params: " + Core.ParamsString);
    Log::Info("Девиз: Чем стрелы коленом ловить, гораздо интереснее отстреливать свои ноги. Продолжим.");
    Log::Info("Slogan: It's more interesting to shoot your feet, than catch arrows by your knee. Let's continue.");
    Core.GetParamsHelp();

    Console.ExecuteConfig();
    Log::ErrorIf(!glfwInit(), "GLFW not initialized.");

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
