#pragma  once
#ifndef xdRender_hpp__
#define xdRender_hpp__
#include "xdRender_impexp.inl"

#include <vulkan/vulkan.hpp>
//#include <GLFW/glfw3.h>

class RENDERER_API Renderer
{
public:
    //vk::ApplicationInfo applicationInfo;
    //vk::InstanceCreateInfo instanceInfo;
    vk::Instance instance;
    vk::DebugReportCallbackEXT vkDebugCallback;
    vk::PhysicalDevice physDevice;
    vk::Device device;
    vk::Queue graphicsQueue;
    vk::SurfaceKHR surface;
    vk::Result result;

    Renderer();
    void Initialize();
    void Destroy();

    void CreateVkInstance();
    std::vector<const char*> getRequiredExtensions();
    bool CheckValidationLayersSupport();
    void SetupDebugCallback();

    void GetPhysDevice();
    bool isPhysDeviceSuitable(vk::PhysicalDevice _device);
    void CreateDevice();

    void CreateVkSurface();
};

#endif // xdRender_hpp__

extern RENDERER_API Renderer Render;
