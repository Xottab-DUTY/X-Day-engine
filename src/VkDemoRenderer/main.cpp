// 01.01.2017 Султан Xottab_DUTY Урамаев
// Чем стрелы коленом ловить, гораздо интереснее отстреливать свои ноги. Продолжим.

#include "pch.hpp"

#include "xdCore/xdCore.hpp"
#include "xdCore/CommandLine/Keys.hpp"
#include "VkDemoEngine.hpp"

int main(int argc, char* argv[])
{
    Core.InitializeArguments(argc, argv);

#ifdef WINDOWS
    SetConsoleCP(65001);
    SetConsoleOutputCP(65001);
    if (XDay::CommandLine::KeyDontHideSystemConsole.IsSet())
        FreeConsole();
#endif

    Core.Initialize();

    XDay::Log::Info(Core.GetGLFWVersionString());
    XDay::Log::Info(Core.GetBuildString());
    XDay::Log::Info("Core.Params: " + Core.ParamsString);
    XDay::Log::Info("Девиз: Чем стрелы коленом ловить, гораздо интереснее отстреливать свои ноги. Продолжим.");
    XDay::Log::Info("Slogan: It's more interesting to shoot your feet, than catch arrows by your knee. Let's continue.");
    Core.GetParamsHelp();

    if (!glfwInit())
        XDay::Log::Error("GLFW not initialized.");
    
    VkDemo.Initialize();
    VkDemo.createMainWindow();
    VkDemo.mainLoop();

    glfwTerminate();

    return 0;
}
