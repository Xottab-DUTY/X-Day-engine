// 01.01.2017 Султан Xottab_DUTY Урамаев
// Чем стрелы коленом ловить, гораздо интереснее отстреливать свои ноги. Продолжим.
#include <GLFW/glfw3.h>

#include "xdEngine.hpp"
#include "xdCore.hpp"
#include "ConsoleCommand.hpp"
#include "Renderer/VkDemoRenderer.hpp"

using namespace XDay;

XDAY_API XDayEngine Engine;

void XDayEngine::onKeyPress(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if(action == GLFW_PRESS)
        switch(key)
        {
            case GLFW_KEY_ESCAPE: ConsoleCommands->Execute(&ExitCC);
            case GLFW_KEY_ENTER:
                {
                    ConsoleCommands->ExecuteBool(&FullscreenCC, !ConsoleCommands->GetBool(&FullscreenCC));

                    if (ConsoleCommands->GetBool(&FullscreenCC))
                        glfwSetWindowMonitor(Engine.window, Engine.CurrentMonitor, 0, 0, Engine.CurrentMode->width, Engine.CurrentMode->height, Engine.CurrentMode->refreshRate);
                    else
                        glfwSetWindowMonitor(Engine.window, nullptr, 32, 64, Engine.CurrentMode->width - 256, Engine.CurrentMode->height - 256, Engine.CurrentMode->refreshRate);
                }
            default: return;
        }
}

void XDayEngine::onWindowResize(GLFWwindow* window, int width, int height)
{
    if (width || height) 
    {
        VkDemoRenderer* pRender = static_cast<VkDemoRenderer*>(glfwGetWindowUserPointer(window));
        pRender->RecreateSwapChain();
    }
}

void XDayEngine::onWindowFocus(GLFWwindow* window, int focused)
{
    if (!focused)
    {
        glfwWaitEvents();
    }
    //else //TODO: The window gained input focus
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
    if (ConsoleCommands->GetBool(&FullscreenCC))
        window = glfwCreateWindow(CurrentMode->width, CurrentMode->height, Core.AppName.c_str(), CurrentMonitor, nullptr);
    else
        window = glfwCreateWindow(CurrentMode->width-256, CurrentMode->height-256, Core.AppName.c_str(), nullptr, nullptr);

    glfwSetKeyCallback(window, onKeyPress);
    glfwSetWindowSizeCallback(window, onWindowResize);
    glfwSetWindowFocusCallback(window, onWindowFocus);
}
