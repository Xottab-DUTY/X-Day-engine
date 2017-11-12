// 01.01.2017 Султан Xottab_DUTY Урамаев
// Чем стрелы коленом ловить, гораздо интереснее отстреливать свои ноги. Продолжим.

#include "pch.hpp"

#include "xdCore/xdCore.hpp"
#include "VkDemoEngine.hpp"

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

    Info(Core.GetGLFWVersionString());
    Info(Core.GetBuildString());
    Info("Core.Params: " + Core.ParamsString);
    Info("Девиз: Чем стрелы коленом ловить, гораздо интереснее отстреливать свои ноги. Продолжим.");
    Info("Slogan: It's more interesting to shoot your feet, than catch arrows by your knee. Let's continue.");
    Core.GetParamsHelp();

    if (!glfwInit())
        Error("GLFW not initialized.");
    
    VkDemo.Initialize();
    VkDemo.createMainWindow();
    VkDemo.mainLoop();

    glfwTerminate();

    return 0;
}
