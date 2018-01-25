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
    const bool allowed = Core.FindParam(CoreParams::DontHideSystemConsole);

    while (allowed && !glfwWindowShouldClose(Engine.windowMain))
    {
        std::string input;
        std::getline(std::cin, input);
        if (!glfwWindowShouldClose(Engine.windowMain) && !input.empty())
            Console.Execute(input);
    }
}

void AttachRenderer()
{
    ModuleManager::LoadModule(EngineModules::Renderer);

    using pFunc = void(*)();
    const auto func = static_cast<pFunc>(ModuleManager::GetProcFromModule(EngineModules::Renderer, "InitializeRenderer"));
    if (func)
        func();
    else
        Log::Error("Cannot attach function InitializeRenderer from {}", ModuleManager::GetModuleName(EngineModules::Renderer));
}

int main(int argc, char* argv[])
{
    Core.InitializeArguments(argc, argv);

#ifdef WINDOWS
    SetConsoleCP(65001);
    SetConsoleOutputCP(65001);
    if (!Core.FindParam(CoreParams::DontHideSystemConsole))
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
