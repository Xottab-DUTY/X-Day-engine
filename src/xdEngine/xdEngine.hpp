#pragma once

#include <GLFW/glfw3.h>

#include "xdEngine_impexp.inl"

class XDAY_API XDayEngine
{
private:
    int MonitorsCount;
    int VideoModesCount;

public:
    GLFWmonitor** monitors;
    GLFWmonitor* CurrentMonitor;

    const GLFWvidmode* VideoModes;
    const GLFWvidmode* CurrentMode;

    GLFWwindow* window;

    void Initialize();

    void xdCreateWindow();

    int GetMonitorsCount() { return MonitorsCount; };
    int GetVideoModesCount() { return VideoModesCount; };
};

extern XDAY_API XDayEngine Engine;
