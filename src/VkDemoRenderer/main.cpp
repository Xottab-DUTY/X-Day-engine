// 01.01.2017 Султан Xottab_DUTY Урамаев
// Чем стрелы коленом ловить, гораздо интереснее отстреливать свои ноги. Продолжим.

#include "pch.hpp"

#include "xdCore/Core.hpp"
#include "xdCore/CommandLine.hpp"
#include "VkDemoEngine.hpp"

int main(int argc, char* argv[])
{
    Core.InitializeArguments(argc, argv);

#ifdef WINDOWS
    //SetConsoleCP(65001);
    SetConsoleOutputCP(65001);
    if (!XDay::CommandLine::KeyDontHideSystemConsole.IsSet())
        FreeConsole();
#endif

    Core.Initialize();

    XDay::CommandLine::Keys::Help();

    if (!glfwInit())
        XDay::Log::Error("GLFW not initialized.");
    
    VkDemo.Initialize();
    VkDemo.createMainWindow();
    VkDemo.mainLoop();

    glfwTerminate();

    return 0;
}
