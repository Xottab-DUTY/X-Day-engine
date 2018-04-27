// 01.01.2017 Султан Xottab_DUTY Урамаев
// Чем стрелы коленом ловить, гораздо интереснее отстреливать свои ноги. Продолжим.
#include "pch.hpp"

#include <iostream>
#include <thread>

#include <GLFW/glfw3.h>

#include "xdCore/Core.hpp"
#include "xdCore/CommandLine/Keys.hpp"
#include "xdEngine/Console/Console.hpp"
#include "xdEngine/xdEngine.hpp"
#include "xdCore/ModuleManager.hpp"

using namespace XDay;

void watch_console()
{
    const bool allowed = CommandLine::KeyDontHideSystemConsole.IsSet();

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
    if (!CommandLine::KeyDontHideSystemConsole.IsSet())
        FreeConsole();
#endif

    Core.Initialize();
    Console.Initialize();

    Log::Debug(Core.GetGLFWVersionString());
    Log::Info(Core.GetBuildString());
    Log::Info("Core.Params: " + Core.GetParamsString());
    Log::Info("Девиз: Чем стрелы коленом ловить, гораздо интереснее отстреливать свои ноги. Продолжим.");
    Log::Info("Slogan: It's more interesting to shoot your feet, than catch arrows by your knee. Let's continue.");
    CommandLine::Keys::Help();

    Console.ExecuteConfig();
    if (!glfwInit())
    {
        Log::Error("GLFW not initialized.");
        // XXX: Crash here
    }

    AttachRenderer();

    Engine.Initialize();
    Engine.createMainWindow();

    std::thread WatchConsole(watch_console);

    Engine.InitRender();

    Engine.mainLoop();

    Core.Destroy();

    WatchConsole.~thread();

    glfwTerminate();

    return 0;
}
