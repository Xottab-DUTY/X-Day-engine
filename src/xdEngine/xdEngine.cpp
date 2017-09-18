// 01.01.2017 Султан Xottab_DUTY Урамаев
// Чем стрелы коленом ловить, гораздо интереснее отстреливать свои ноги. Продолжим.
#include <GLFW/glfw3.h>

#include "xdEngine.hpp"
#include "xdCore.hpp"
#include "ConsoleCommand.hpp"
#include "ConsoleCommands.hpp"
#include "xdAPI/xdAPI.hpp"
#include "xdRenderer/renderer.hpp"
#include "Renderer/VkDemoRenderer.hpp"
#include "Debug/Log.hpp"

using namespace XDay;

XDAY_API XDayEngine Engine;

void XDayEngine::onKeyPress(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if(action == GLFW_PRESS)
    {
        switch (key)
        {
        case GLFW_KEY_ESCAPE:
        {
            ConsoleCommands->Execute(&ExitCC);
            break;
        }
        case GLFW_KEY_ENTER:
        {
            ConsoleCommands->ExecuteBool(&FullscreenCC, !ConsoleCommands->GetBool(&FullscreenCC));
            if (ConsoleCommands->GetBool(&FullscreenCC))
                glfwSetWindowMonitor(Engine.windowMain, Engine.currentMonitor, 0, 0, Engine.currentMode->width, Engine.currentMode->height, Engine.currentMode->refreshRate);
            else
                glfwSetWindowMonitor(Engine.windowMain, nullptr, 32, 64, Engine.currentMode->width-256, Engine.currentMode->height-256, Engine.currentMode->refreshRate);
            break;
        }
        default:
        {
            DebugMsg("Key pressed: {}", glfwGetKeyName(key, scancode));
            break;
        }
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
        windowMain = glfwCreateWindow(currentMode->width, currentMode->height, Core.AppName.c_str(), currentMonitor, nullptr);
    else
        windowMain = glfwCreateWindow(currentMode->width-256, currentMode->height-256, Core.AppName.c_str(), nullptr, nullptr);

    glfwSetKeyCallback(windowMain, onKeyPress);
    glfwSetWindowSizeCallback(windowMain, onWindowResize);
    glfwSetWindowFocusCallback(windowMain, onWindowFocus);
    glfwSetMouseButtonCallback(windowMain, onMouseButton);
    glfwSetCursorPosCallback(windowMain, onCursorPosition);
    glfwSetScrollCallback(windowMain, onMouseScroll);
}

void XDayEngine::InitRender()
{
    GEnv.Render->initialize();
    VkDemoRender.Initialize();
}

void XDayEngine::mainLoop()
{
    while (!glfwWindowShouldClose(windowMain))
    {
        glfwPollEvents();

        while (VkDemoRender.renderPaused == true)
            glfwWaitEvents();

        VkDemoRender.UpdateUniformBuffer();
        VkDemoRender.DrawFrame();
    }
    VkDemoRender.device->waitIdle();
}
