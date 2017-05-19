#pragma once
#ifndef xdRender_hpp__
#define xdRender_hpp__
#include <vulkan/vulkan.hpp>

#include "xdEngine_impexp.inl"

class XDAY_API Renderer
{
public:
    vk::Instance instance;
    vk::DebugReportCallbackEXT vkDebugCallback;
    vk::PhysicalDevice physDevice;
    vk::Device device;
    vk::Queue graphicsQueue;
    vk::Queue presentQueue;
    vk::SurfaceKHR surface;
    vk::Result result;

    struct QueueFamilyIndices;
    QueueFamilyIndices findQueueFamilies(vk::PhysicalDevice device) const;

    Renderer();
    void Initialize();
    void Destroy();

    void CreateVkInstance();
    std::vector<const char*> getRequiredExtensions();
    bool CheckValidationLayersSupport();
    void SetupDebugCallback();

    void GetPhysDevice();
    bool isPhysDeviceSuitable(vk::PhysicalDevice _device) const;
    void CreateDevice();

    void CreateVkSurface();
};

extern XDAY_API Renderer Render;

#endif // xdRender_hpp__
