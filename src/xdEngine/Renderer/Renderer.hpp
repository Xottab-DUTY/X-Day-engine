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
    QueueFamilyIndices findQueueFamilies(vk::PhysicalDevice _physDevice) const;

    struct SwapChainSupportDetails;
    SwapChainSupportDetails querySwapChainSupport(vk::PhysicalDevice _physDevice) const;

    Renderer();
    void Initialize();
    void Destroy();

    void CreateVkInstance();
    bool CheckValidationLayersSupport();
    void SetupDebugCallback();

    void GetPhysDevice();
    bool isPhysDeviceSuitable(vk::PhysicalDevice _physDevice) const;
    bool checkDeviceExtensionSupport(vk::PhysicalDevice _physDevice) const;
    void CreateDevice();

    void CreateVkSurface();
};

extern XDAY_API Renderer Render;

#endif // xdRender_hpp__
