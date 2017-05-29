#pragma once
#ifndef xdRender_hpp__
#define xdRender_hpp__
#include "Common/Platform.hpp" // this must be first
#include <vulkan/vulkan.hpp>

#include "xdEngine_impexp.inl"
#include <ResourceLimits.h>

class XDAY_API Renderer
{
public:
    vk::Instance instance;
    vk::DebugReportCallbackEXT vkCallback;
    vk::PhysicalDevice physDevice;
    vk::Device device;
    vk::Queue graphicsQueue;
    vk::Queue presentQueue;
    vk::SurfaceKHR surface;
    vk::Result result;
    vk::SwapchainKHR swapchain;
    std::vector<vk::Image> swapChainImages;
    vk::Format swapChainImageFormat;
    vk::Extent2D swapChainExtent;
    std::vector<vk::ImageView> swapChainImageViews;
    vk::RenderPass renderPass;
    vk::PipelineLayout pipelineLayout;
    vk::Pipeline graphicsPipeline;
    std::vector<vk::Framebuffer> swapChainFramebuffers;
    vk::CommandPool commandPool;
    std::vector<vk::CommandBuffer> commandBuffers;

    TBuiltInResource resources;

    struct QueueFamilyIndices;
    struct SwapChainSupportDetails;

    Renderer();
    void Initialize();
    void Destroy();

private:
    void InitializeResources();
    void CreateVkInstance();
    bool CheckValidationLayersSupport() const;
    std::vector<const char*> getRequiredExtensions();
    void CreateDebugCallback();

    void CreateVkSurface();

    void GetPhysDevice();
    QueueFamilyIndices findQueueFamilies(vk::PhysicalDevice _physDevice) const;
    SwapChainSupportDetails querySwapChainSupport(vk::PhysicalDevice _physDevice) const;
    bool isPhysDeviceSuitable(vk::PhysicalDevice _physDevice) const;

    bool checkDeviceExtensionSupport(vk::PhysicalDevice _physDevice) const;
    void CreateDevice();

    vk::SurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& availableFormats) const;
    vk::PresentModeKHR chooseSwapPresentMode(const std::vector<vk::PresentModeKHR> availablePresentModes) const;
    vk::Extent2D chooseSwapExtent(const vk::SurfaceCapabilitiesKHR& capabilities) const;
    void CreateSwapChain();
    void CreateImageViews();
    void CreateRenderPass();
    void CreateGraphicsPipeline();
    void CreateFramebuffers();
    void CreateCommandPool();
    void CreateCommandBuffers();
};

extern XDAY_API Renderer Render;

#endif // xdRender_hpp__
