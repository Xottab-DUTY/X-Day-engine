// 01.01.2017 Султан Xottab_DUTY Урамаев
// Чем стрелы коленом ловить, гораздо интереснее отстреливать свои ноги. Продолжим.
#include "pch.hpp"

#include <iostream>
#include <thread>

#include <GLFW/glfw3.h>

#include "xdCore/Core.hpp"
#include "xdCore/CommandLine.hpp"
#include "xdCore/Console/ConsoleCommands.hpp"
#include "xdEngine/Console/Console.hpp"
#include "xdEngine/xdEngine.hpp"

using namespace XDay;

void watch_console()
{
    const bool allowed = CommandLine::KeyDontHideSystemConsole.IsSet();

    while (allowed && !glfwWindowShouldClose(Engine.windowMain))
    {
        std::string input;
        std::getline(std::cin, input);
        if (!glfwWindowShouldClose(Engine.windowMain) && !input.empty())
            Console::Commands::Execute(input.c_str());
    }
}

int main(int argc, char* argv[])
{
    Core.InitializeArguments(argc, argv);

#ifdef WINDOWS
    //SetConsoleCP(65001);
    SetConsoleOutputCP(65001);
    if (!CommandLine::KeyDontHideSystemConsole.IsSet())
        FreeConsole();
#endif

    Core.Initialize();
    Console::Console.Initialize();

    CommandLine::Keys::Help();

    Console::Console.ExecuteConfig();
    if (!glfwInit())
    {
        Log::Error("GLFW not initialized.");
        // XXX: Crash here
    }

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
