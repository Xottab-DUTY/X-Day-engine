#pragma once
#ifndef xdEngine_hpp__
#define xdEngine_hpp__

#include <GLFW/glfw3.h>

#include "Common/import_export_macros.hpp"

namespace XDay
{
class XDAY_API XDayEngine
{
    int MonitorsCount;
    int VideoModesCount;

public:
    GLFWmonitor** monitors;
    GLFWmonitor* currentMonitor;

    const GLFWvidmode* videoModes;
    const GLFWvidmode* currentMode;

    GLFWwindow* window;
    bool windowMainFullscreen;

    void Initialize();
    void createMainWindow();

    void InitRender();

    void mainLoop();

    static void onWindowResize(GLFWwindow* window, int width, int height);
    static void onKeyPress(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void onMouseButton(GLFWwindow* window, int button, int action, int mods);
    static void onCursorPosition(GLFWwindow* window, double xpos, double ypos);
    static void onWindowFocus(GLFWwindow* window, int focused);
    static void onMouseScroll(GLFWwindow* window, double xoffset, double yoffset);
    static void onMouseEnter(GLFWwindow* window, int entered);

    int getMonitorsCount() const { return MonitorsCount; }
    int getVideoModesCount() const { return VideoModesCount; }

    bool isWindowMainFullscreen() const { return windowMainFullscreen; }
    void setWindowMainFullscreen(bool fullscreen) { windowMainFullscreen = fullscreen; }
};
}

extern XDAY_API XDay::XDayEngine Engine;

#endif // xdEngine_hpp__
