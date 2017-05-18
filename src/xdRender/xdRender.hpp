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
    vk::PhysicalDevice physDevice;
    vk::Device device;
    vk::SurfaceKHR surface;
    vk::Result result;

    Renderer();
    void Initialize();

    void CreateVkInstance();
    bool CheckValidationLayersSupport();
    std::vector<const char*> getRequiredExtensions();

    bool isPhysDeviceSuitable(vk::PhysicalDevice _device);
    void GetPhysDevice();
    void CreateDevice();

    void CreateVkSurface();

    void Destroy();
};

#endif // xdRender_hpp__

extern RENDERER_API Renderer Render;
