// 01.01.2017 Султан Xottab_DUTY Урамаев
// Чем стрелы коленом ловить, гораздо интереснее отстреливать свои ноги. Продолжим.
#include <GLFW/glfw3.h>

#include "xdEngine.hpp"
#include "xdCore.hpp"
#include "ConsoleCommand.hpp"
#include "ConsoleCommands.hpp"
#include "Renderer/VkDemoRenderer.hpp"

using namespace XDay;

XDAY_API XDayEngine Engine;

void XDayEngine::onKeyPress(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if(action == GLFW_PRESS)
    {
        switch (key)
        {
        case GLFW_KEY_ESCAPE:
            ConsoleCommands->Execute(&ExitCC);
        case GLFW_KEY_ENTER:
        {
            ConsoleCommands->ExecuteBool(&FullscreenCC, !ConsoleCommands->GetBool(&FullscreenCC));
            if (ConsoleCommands->GetBool(&FullscreenCC))
                glfwSetWindowMonitor(Engine.window, Engine.currentMonitor, 0, 0, Engine.currentMode->width, Engine.currentMode->height, Engine.currentMode->refreshRate);
            else
                glfwSetWindowMonitor(Engine.window, nullptr, 32, 64, Engine.currentMode->width-256, Engine.currentMode->height-256, Engine.currentMode->refreshRate);
        }
        default: return;
        }
    }
        
}

void XDayEngine::onWindowResize(GLFWwindow* window, int width, int height)
{
    VkDemoRenderer* pRender = static_cast<VkDemoRenderer*>(glfwGetWindowUserPointer(window));
    if (width < 108 || height < 108)
    {
        pRender->renderPaused = true;
        return;
    }
    pRender->renderPaused = false;

    pRender->RecreateSwapChain();
}

void XDayEngine::onWindowFocus(GLFWwindow* window, int focused)
{
    VkDemoRenderer* pRender = static_cast<VkDemoRenderer*>(glfwGetWindowUserPointer(window));

    int currentWidth, currentHeight;
    glfwGetWindowSize(window, &currentWidth, &currentHeight);

    if (focused && !(currentWidth < 108 || currentHeight < 108))
        pRender->renderPaused = false;
    else
        pRender->renderPaused = true;
}

void XDayEngine::onMouseButton(GLFWwindow* window, int button, int action, int mods)
{

}

void XDayEngine::onCursorPosition(GLFWwindow* window, double xpos, double ypos)
{

}

void XDayEngine::onMouseScroll(GLFWwindow* window, double xoffset, double yoffset)
{

}

void XDayEngine::onMouseEnter(GLFWwindow* window, int entered)
{

}


void XDayEngine::Initialize()
{
    monitors = glfwGetMonitors(&MonitorsCount);
    currentMonitor = glfwGetPrimaryMonitor();

    videoModes = glfwGetVideoModes(currentMonitor, &VideoModesCount);
    currentMode = glfwGetVideoMode(currentMonitor);

    glfwWindowHint(GLFW_RED_BITS, currentMode->redBits);
    glfwWindowHint(GLFW_GREEN_BITS, currentMode->greenBits);
    glfwWindowHint(GLFW_BLUE_BITS, currentMode->blueBits);
    glfwWindowHint(GLFW_REFRESH_RATE, currentMode->refreshRate);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
}

void XDayEngine::createMainWindow()
{
    if (ConsoleCommands->GetBool(&FullscreenCC))
        window = glfwCreateWindow(currentMode->width, currentMode->height, Core.AppName.c_str(), currentMonitor, nullptr);
    else
        window = glfwCreateWindow(currentMode->width-256, currentMode->height-256, Core.AppName.c_str(), nullptr, nullptr);

    glfwSetKeyCallback(window, onKeyPress);
    glfwSetWindowSizeCallback(window, onWindowResize);
    glfwSetWindowFocusCallback(window, onWindowFocus);
    glfwSetMouseButtonCallback(window, onMouseButton);
    glfwSetCursorPosCallback(window, onCursorPosition);
    glfwSetScrollCallback(window, onMouseScroll);
}
