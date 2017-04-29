// 01.01.2017 Султан Xottab_DUTY Урамаев
// Чем стрелы коленом ловить, гораздо интереснее отстреливать свои ноги. Продолжим.
#include <GLFW/glfw3.h>

#include "xdEngine.hpp"
#include "xdCore.hpp"
#include "ConsoleCommand.hpp"

XDAY_API XDayEngine Engine;

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        ConsoleCommands->Execute(&ExitCC);
    if (key == GLFW_KEY_ENTER && action == GLFW_PRESS)
        ConsoleCommands->ExecuteBool(&FullscreenCC, !ConsoleCommands->GetBool(&FullscreenCC));
}

void XDayEngine::Initialize()
{
    monitors = glfwGetMonitors(&MonitorsCount);
    CurrentMonitor = glfwGetPrimaryMonitor();

    VideoModes = glfwGetVideoModes(CurrentMonitor, &VideoModesCount);
    CurrentMode = glfwGetVideoMode(CurrentMonitor);

    glfwWindowHint(GLFW_RED_BITS, CurrentMode->redBits);
    glfwWindowHint(GLFW_GREEN_BITS, CurrentMode->greenBits);
    glfwWindowHint(GLFW_BLUE_BITS, CurrentMode->blueBits);
    glfwWindowHint(GLFW_REFRESH_RATE, CurrentMode->refreshRate);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
}

void XDayEngine::xdCreateWindow()
{
    if (ConsoleCommands->GetBool("r_fullscreen"))
        window = glfwCreateWindow(CurrentMode->width, CurrentMode->height, Core.AppName.c_str(), CurrentMonitor, nullptr);
    else
        window = glfwCreateWindow(CurrentMode->width-256, CurrentMode->height-256, Core.AppName.c_str(), nullptr, nullptr);
    glfwSetKeyCallback(window, key_callback);
}
