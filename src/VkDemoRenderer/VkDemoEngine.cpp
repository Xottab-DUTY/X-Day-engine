#include "pch.hpp"
#include "VkDemoEngine.hpp"
#include "VkDemoRenderer.hpp"

VkDemoEngine VkDemo;

void VkDemoEngine::Initialize()
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

void VkDemoEngine::createMainWindow()
{
    windowDemo = glfwCreateWindow(currentMode->width-256, currentMode->height-256, "Vulkan Demo Renderer", nullptr, nullptr);

    glfwSetWindowSizeLimits(windowDemo, 256, 256, GLFW_DONT_CARE, GLFW_DONT_CARE);
    glfwSetWindowFocusCallback(windowDemo, onWindowFocus);
    glfwSetWindowRefreshCallback(windowDemo, onWindowRefresh);

    glfwSetKeyCallback(windowDemo, onKeyPress);
}

void VkDemoEngine::mainLoop()
{
    VkDemoRender.Initialize();
    while (!glfwWindowShouldClose(windowDemo))
    {
        glfwPollEvents();

        while (VkDemoRender.renderPaused == true)
            glfwWaitEvents();

        VkDemoRender.UpdateUniformBuffer();
        VkDemoRender.DrawFrame();
    }
    VkDemoRender.device->waitIdle();
}

void VkDemoEngine::onWindowFocus(GLFWwindow* window, int focused)
{
    if (focused)
        VkDemoRender.renderPaused = false;
    else
        VkDemoRender.renderPaused = true;
}

void VkDemoEngine::onWindowRefresh(GLFWwindow* window)
{
    VkDemoRender.RecreateSwapChain();
}

void VkDemoEngine::onKeyPress(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (action == GLFW_PRESS)
    {
        switch (key)
        {
        case GLFW_KEY_ESCAPE:
        {
            glfwSetWindowShouldClose(VkDemo.windowDemo, true);
            break;
        }
        case GLFW_KEY_ENTER:
        {
            VkDemo.windowDemoFullscreen = !VkDemo.windowDemoFullscreen;
            if (VkDemo.windowDemoFullscreen)
                glfwSetWindowMonitor(VkDemo.windowDemo, VkDemo.currentMonitor, 0, 0, VkDemo.currentMode->width,
                                     VkDemo.currentMode->height, VkDemo.currentMode->refreshRate);
            else
                glfwSetWindowMonitor(VkDemo.windowDemo, nullptr, 32, 64, VkDemo.currentMode->width-256,
                                     VkDemo.currentMode->height-256, VkDemo.currentMode->refreshRate);
            break;
        }
        }
    }
}
