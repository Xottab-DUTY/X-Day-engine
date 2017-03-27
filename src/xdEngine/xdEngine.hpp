#pragma once

#include <GLFW/glfw3.h>

#include "Platform/Platform.hpp"

#ifdef XDENGINE_EXPORTS
#define XDENGINE_API XD_EXPORT
#else
#define XDENGINE_API XD_IMPORT
#endif

class XDENGINE_API XDayEngine
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

extern XDENGINE_API XDayEngine Engine;
