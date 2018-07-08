// 01.01.2017 Султан Xottab_DUTY Урамаев
// Чем стрелы коленом ловить, гораздо интереснее отстреливать свои ноги. Продолжим.
#include "pch.hpp"

#include <GLFW/glfw3.h>

#include "xdAPI/xdAPI.hpp"
#include "xdEngine.hpp"
#include "xdCore/Core.hpp"
#include "xdCore/Console/ConsoleCommands.hpp"
#include "Console/ConsoleCommands.hpp"
#include "xdRenderer/renderer.hpp"

using namespace XDay;

XDENGINE_API XDayEngine Engine;

void XDayEngine::onKeyPress(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if(action == GLFW_PRESS)
    {
        switch (key)
        {
        case GLFW_KEY_ESCAPE:
        {
            Console::Quit.Execute();
            break;
        }
        case GLFW_KEY_ENTER:
        {
            Console::Fullscreen.RevertValue();
            if (Console::Fullscreen.Value())
                glfwSetWindowMonitor(Engine.windowMain, Engine.currentMonitor, 0, 0, Engine.currentMode->width, Engine.currentMode->height, Engine.currentMode->refreshRate);
            else
                glfwSetWindowMonitor(Engine.windowMain, nullptr, 32, 64, Engine.currentMode->width-256, Engine.currentMode->height-256, Engine.currentMode->refreshRate);
            break;
        }
        default:
        {
            Log::Trace("Key pressed: {}", glfwGetKeyName(key, scancode));
            break;
        }
        }
    }
}

void XDayEngine::onWindowResize(GLFWwindow* window, int width, int height)
{}

void XDayEngine::onWindowFocus(GLFWwindow* window, int focused)
{}

void XDayEngine::onWindowRefresh(GLFWwindow* window)
{}

void XDayEngine::onMouseButton(GLFWwindow* window, int button, int action, int mods)
{}

void XDayEngine::onMouseScroll(GLFWwindow* window, double xoffset, double yoffset)
{}

void XDayEngine::onCursorEnter(GLFWwindow* window, int entered)
{}

void XDayEngine::onCursorPosition(GLFWwindow* window, double xpos, double ypos)
{}

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
    if (Console::Fullscreen.Value())
        windowMain = glfwCreateWindow(currentMode->width, currentMode->height, Core.GetAppName().c_str(), currentMonitor, nullptr);
    else
        windowMain = glfwCreateWindow(currentMode->width-256, currentMode->height-256, Core.GetAppName().c_str(), nullptr, nullptr);

    glfwSetWindowSizeLimits(windowMain, 256, 256, GLFW_DONT_CARE, GLFW_DONT_CARE);

    //glfwSetWindowSizeCallback(windowMain, onWindowResize);
    glfwSetWindowFocusCallback(windowMain, onWindowFocus);
    glfwSetWindowRefreshCallback(windowMain, onWindowRefresh);

    glfwSetKeyCallback(windowMain, onKeyPress);

    //glfwSetMouseButtonCallback(windowMain, onMouseButton);
    //glfwSetScrollCallback(windowMain, onMouseScroll);

    //glfwSetCursorEnterCallback(windowMain, onCursorEnter);
    //glfwSetCursorPosCallback(windowMain, onCursorPosition);
}

void XDayEngine::InitRender()
{
    //GEnv.Render->Initialize();
}

void XDayEngine::mainLoop()
{
    while (!glfwWindowShouldClose(windowMain))
    {
        glfwWaitEvents();
    }
}

