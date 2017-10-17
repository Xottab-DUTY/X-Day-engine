#pragma once

#include <GLFW/glfw3.h>

class VkDemoEngine
{
    int MonitorsCount;
    int VideoModesCount;

public:
    GLFWmonitor** monitors;
    GLFWmonitor* currentMonitor;

    const GLFWvidmode* videoModes;
    const GLFWvidmode* currentMode;

    GLFWwindow* windowDemo;
    bool windowDemoFullscreen;

    void Initialize();

    void createMainWindow();

    void mainLoop();

    static void onWindowFocus(GLFWwindow* window, int focused);
    static void onWindowRefresh(GLFWwindow* window);
    static void onKeyPress(GLFWwindow* window, int key, int scancode, int action, int mods);
};

extern VkDemoEngine VkDemo;
