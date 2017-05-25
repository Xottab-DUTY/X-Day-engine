#include "Common/Platform.hpp" // this must be first

#include <set>
#include <vulkan/vulkan.hpp>

#include <GLFW/glfw3.h>
#include <fmt/ostream.h>

#include "xdEngine/Debug/Log.hpp"
#include "xdEngine/xdCore.hpp"
#include "xdEngine/xdEngine.hpp"
#include "Renderer.hpp"

XDAY_API Renderer Render;

const bool enableValidationLayers = Core.isGlobalDebug();

const std::vector<const char*> validationLayers =
{
    "VK_LAYER_LUNARG_standard_validation"
};

const std::vector<const char*> deviceExtensions =
{
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

VkResult vkCreateDebugReportCallbackEXT(VkInstance instance, const VkDebugReportCallbackCreateInfoEXT* pCreateInfo,
    const VkAllocationCallbacks* pAllocator, VkDebugReportCallbackEXT* pCallback)
{
    auto func = (PFN_vkCreateDebugReportCallbackEXT)glfwGetInstanceProcAddress(instance, "vkCreateDebugReportCallbackEXT");
    if (func)
        return func(instance, pCreateInfo, pAllocator, pCallback);

    return VK_ERROR_EXTENSION_NOT_PRESENT;
}

void vkDestroyDebugReportCallbackEXT(VkInstance instance, VkDebugReportCallbackEXT callback, const VkAllocationCallbacks* pAllocator = nullptr)
{
    auto func = (PFN_vkDestroyDebugReportCallbackEXT)glfwGetInstanceProcAddress(instance, "vkDestroyDebugReportCallbackEXT");
    if (func && callback)
        func(instance, callback, pAllocator);
}

static VKAPI_ATTR vk::Bool32 VKAPI_CALL vkDebugCallback(
    vk::DebugReportFlagsEXT flags,
    vk::DebugReportObjectTypeEXT objType,
    uint64_t obj,
    size_t location,
    int32_t code,
    const char* layerPrefix,
    const char* msg,
    void* userData)
{
    Msg("\nValidation layer reports: \n" \
        "Flags: not working right now \n" \
        "Object type: {} \n" \
        "Object: {} \n" \
        "Location: {} \n" \
        "Code: {} \n" \
        "Layer prefix: {} \n" \
        "Message: {} \n" \
        "User data: {}\n",
        vk::to_string(objType),
        obj, location,
        code, layerPrefix,
        msg, userData);

    return VK_FALSE;
}

Renderer::Renderer(): result(vk::Result::eNotReady),
    swapChainImageFormat(vk::Format::eUndefined) {}

void Renderer::Initialize()
{
    CreateVkInstance();
    CreateDebugCallback();
    CreateVkSurface();
    GetPhysDevice();
    CreateDevice();
    CreateSwapChain();
    CreateImageViews();
}

void Renderer::Destroy()
{
    for (size_t i = 0; i < swapChainImageViews.size(); ++i)
        device.destroyImageView(swapChainImageViews[i], nullptr);

    device.destroySwapchainKHR(swapchain);
    device.destroy();
    instance.destroySurfaceKHR(surface);
    instance.destroyDebugReportCallbackEXT(vkCallback);
    instance.destroy();
}

void Renderer::CreateVkInstance()
{
    if (enableValidationLayers && !CheckValidationLayersSupport())
        Log("Vulkan: not all validation layers supported.");

    vk::ApplicationInfo appInfo(Core.AppName.c_str(), stoi(Core.AppVersion),
                                Core.EngineName.c_str(), stoi(Core.EngineVersion), 
                                VK_MAKE_VERSION(1, 0, 42));

    vk::InstanceCreateInfo i;
    i.setPApplicationInfo(&appInfo);

    auto extensions = getRequiredExtensions();    
    i.setEnabledExtensionCount(static_cast<uint32_t>(extensions.size()));
    i.setPpEnabledExtensionNames(extensions.data());

    if (enableValidationLayers)
    {
        i.setEnabledLayerCount(static_cast<uint32_t>(validationLayers.size()));
        i.setPpEnabledLayerNames(validationLayers.data());
    }

    result = vk::createInstance(&i, nullptr, &instance);
    assert(result == vk::Result::eSuccess);
}

bool Renderer::CheckValidationLayersSupport() const
{
    std::vector<vk::LayerProperties> availableLayers = vk::enumerateInstanceLayerProperties();

    for (std::string layerName : validationLayers)
    {
        bool layerFound = false;

        for (const auto& layerProperties : availableLayers)
            if (layerName.compare(layerProperties.layerName))
            {
                layerFound = true;
                break;
            }

        if (!layerFound)
            return false;
    }

    return true;
}

std::vector<const char*> Renderer::getRequiredExtensions()
{
    std::vector<const char*> extensions;

    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    for (uint32_t i = 0; i < glfwExtensionCount; i++)
        extensions.push_back(glfwExtensions[i]);

    if (enableValidationLayers)
        extensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);

    return extensions;
}

void Renderer::CreateDebugCallback()
{
    if (!enableValidationLayers) return;

    vk::DebugReportCallbackCreateInfoEXT callbackInfo
        ({vk::DebugReportFlagBitsEXT::eError | vk::DebugReportFlagBitsEXT::eWarning},
        (PFN_vkDebugReportCallbackEXT)vkDebugCallback);

    result = instance.createDebugReportCallbackEXT(&callbackInfo, nullptr, &vkCallback);
    assert(result == vk::Result::eSuccess);
}

void Renderer::CreateVkSurface()
{
    result = (vk::Result)glfwCreateWindowSurface(
        (VkInstance)instance, Engine.window,
        nullptr, (VkSurfaceKHR*)&surface);

    assert(result == vk::Result::eSuccess);
}

struct Renderer::QueueFamilyIndices
{
    int graphicsFamily = -1;
    int presentFamily = -1;

    bool isComplete() const
    {
        return graphicsFamily >= 0 && presentFamily >= 0;
    }
};

Renderer::QueueFamilyIndices Renderer::findQueueFamilies(vk::PhysicalDevice _physDevice) const
{
    QueueFamilyIndices indices;

    std::vector<vk::QueueFamilyProperties> queueFamilies = _physDevice.getQueueFamilyProperties();

    int i = 0;
    for (const auto& queueFamily : queueFamilies)
    {
        if (queueFamily.queueCount > 0 && queueFamily.queueFlags & vk::QueueFlagBits::eGraphics)
            indices.graphicsFamily = i;

        vk::Bool32 presentSupport = false;
        _physDevice.getSurfaceSupportKHR(i, surface, &presentSupport);

        if (queueFamily.queueCount > 0 && presentSupport)
            indices.presentFamily = i;

        if (indices.isComplete())
            break;

        ++i;
    }

    return indices;
}

struct Renderer::SwapChainSupportDetails
{
    vk::SurfaceCapabilitiesKHR capabilities;
    std::vector<vk::SurfaceFormatKHR> formats;
    std::vector<vk::PresentModeKHR> presentModes;
};

Renderer::SwapChainSupportDetails Renderer::querySwapChainSupport(vk::PhysicalDevice _physDevice) const
{
    SwapChainSupportDetails details;

    _physDevice.getSurfaceCapabilitiesKHR(surface, &details.capabilities);

    {
        uint32_t formatCount;
        _physDevice.getSurfaceFormatsKHR(surface, &formatCount, nullptr);

        if (formatCount != 0)
        {
            details.formats.resize(static_cast<size_t>(formatCount));
            _physDevice.getSurfaceFormatsKHR(surface, &formatCount, details.formats.data());
        }
    }

    {
        uint32_t presentModeCount;
        _physDevice.getSurfacePresentModesKHR(surface, &presentModeCount, nullptr);

        if (presentModeCount != 0)
        {
            details.presentModes.resize(static_cast<size_t>(presentModeCount));
            _physDevice.getSurfacePresentModesKHR(surface, &presentModeCount, details.presentModes.data());
        }
    }

    return details;
}

void Renderer::GetPhysDevice()
{
    std::vector<vk::PhysicalDevice> physDevices = instance.enumeratePhysicalDevices();
    for (const auto& _device : physDevices)
        if (isPhysDeviceSuitable(_device))
        {
            physDevice = _device;
            break;
        }

    assert(physDevice);
}

bool Renderer::isPhysDeviceSuitable(vk::PhysicalDevice _physDevice) const
{
    QueueFamilyIndices indices = findQueueFamilies(_physDevice);

    bool extensionsSupported = checkDeviceExtensionSupport(_physDevice);

    bool swapChainAdequate = false;
    if (extensionsSupported)
    {
        SwapChainSupportDetails swapChainSupport = querySwapChainSupport(_physDevice);
        swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
    }

    return indices.isComplete() && extensionsSupported && swapChainAdequate;
}

bool Renderer::checkDeviceExtensionSupport(vk::PhysicalDevice _physDevice) const
{
    std::vector<vk::ExtensionProperties> availableExtensions = _physDevice.enumerateDeviceExtensionProperties();

    std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

    for (const auto& extension : availableExtensions)
        requiredExtensions.erase(extension.extensionName);

    return requiredExtensions.empty();
}

void Renderer::CreateDevice()
{
    QueueFamilyIndices indices = findQueueFamilies(physDevice);

    std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;
    std::set<int> uniqueQueueFamilies = { indices.graphicsFamily, indices.presentFamily };

    float queuePriority = 1.0f;
    for (int queueFamily : uniqueQueueFamilies)
    {
        vk::DeviceQueueCreateInfo queueCreateInfo({}, queueFamily, 1, &queuePriority);
        queueCreateInfos.push_back(queueCreateInfo);
    }
    
    vk::PhysicalDeviceFeatures deviceFeatures = {};

    vk::DeviceCreateInfo deviceCreateInfo;
    deviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
    deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();
    deviceCreateInfo.pEnabledFeatures = &deviceFeatures;
    deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
    deviceCreateInfo.ppEnabledExtensionNames = deviceExtensions.data();

    if (enableValidationLayers)
    {
        deviceCreateInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        deviceCreateInfo.ppEnabledLayerNames = validationLayers.data();
    }
    else
        deviceCreateInfo.enabledLayerCount = 0;

    device = physDevice.createDevice(deviceCreateInfo);
    assert(device);

    device.getQueue(indices.graphicsFamily, 0, &graphicsQueue);
    device.getQueue(indices.presentFamily, 0, &presentQueue);
}

vk::SurfaceFormatKHR Renderer::chooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& availableFormats) const
{
    if (availableFormats.size() == 1 && availableFormats[0].format == vk::Format::eUndefined)
        return { vk::Format::eB8G8R8A8Unorm, vk::ColorSpaceKHR::eSrgbNonlinear };

    for (const auto& availableFormat : availableFormats)
        if (availableFormat.format == vk::Format::eB8G8R8A8Unorm && availableFormat.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear)
            return availableFormat;

    return availableFormats[0];
}

vk::PresentModeKHR Renderer::chooseSwapPresentMode(const std::vector<vk::PresentModeKHR> availablePresentModes) const
{
    for (const auto& availablePresentMode : availablePresentModes)
        if (availablePresentMode == vk::PresentModeKHR::eMailbox)
            return availablePresentMode;

    return vk::PresentModeKHR::eFifo;
}

vk::Extent2D Renderer::chooseSwapExtent(const vk::SurfaceCapabilitiesKHR& capabilities) const
{
    if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
        return capabilities.currentExtent;

    vk::Extent2D actualExtent = { static_cast<uint32_t>(Engine.CurrentMode->width), static_cast<uint32_t>(Engine.CurrentMode->height) };

    actualExtent.width = std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, actualExtent.width));
    actualExtent.height = std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, actualExtent.height));

    return actualExtent;
}

void Renderer::CreateSwapChain()
{
    SwapChainSupportDetails swapChainSupport = querySwapChainSupport(physDevice);

    vk::SurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
    vk::PresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
    vk::Extent2D extent = chooseSwapExtent(swapChainSupport.capabilities);

    uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
    if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount)
        imageCount = swapChainSupport.capabilities.maxImageCount;

    vk::SwapchainCreateInfoKHR swapchainInfo = {};
    swapchainInfo.setSurface(surface);
    swapchainInfo.setMinImageCount(imageCount);
    swapchainInfo.setImageFormat(surfaceFormat.format);
    swapchainInfo.setImageColorSpace(surfaceFormat.colorSpace);
    swapchainInfo.setImageExtent(extent);
    swapchainInfo.setImageArrayLayers(1);
    swapchainInfo.setImageUsage(vk::ImageUsageFlagBits::eColorAttachment);

    QueueFamilyIndices indices = findQueueFamilies(physDevice);
    uint32_t queueFamilyIndices[] = { static_cast<uint32_t>(indices.graphicsFamily), static_cast<uint32_t>(indices.presentFamily) };

    if (indices.graphicsFamily != indices.presentFamily)
    {
        swapchainInfo.setImageSharingMode(vk::SharingMode::eConcurrent);
        swapchainInfo.setQueueFamilyIndexCount(2);
        swapchainInfo.setPQueueFamilyIndices(queueFamilyIndices);
    }
    else
    {
        swapchainInfo.setImageSharingMode(vk::SharingMode::eExclusive);
        swapchainInfo.setQueueFamilyIndexCount(0); // Optional
        swapchainInfo.setPQueueFamilyIndices(nullptr); // Optional
    }

    swapchainInfo.setPreTransform(swapChainSupport.capabilities.currentTransform);
    swapchainInfo.setCompositeAlpha(vk::CompositeAlphaFlagBitsKHR::eOpaque);
    swapchainInfo.setPresentMode(presentMode);
    swapchainInfo.setClipped(true);
    swapchainInfo.setOldSwapchain(nullptr);

    result = device.createSwapchainKHR(&swapchainInfo, nullptr, &swapchain);
    assert(result == vk::Result::eSuccess);

    swapChainImages = device.getSwapchainImagesKHR(swapchain);
    swapChainImageFormat = surfaceFormat.format;
    swapChainExtent = extent;
}

void Renderer::CreateImageViews()
{
    swapChainImageViews.resize(swapChainImages.size());

    for (size_t i = 0; i < swapChainImages.size(); ++i)
    {
        vk::ImageSubresourceRange range(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1);
        vk::ImageViewCreateInfo imageViewInfo({}, swapChainImages[i], vk::ImageViewType::e2D,
            swapChainImageFormat, vk::ComponentSwizzle::eIdentity, range);

        result = device.createImageView(&imageViewInfo, nullptr, &swapChainImageViews[i]);
        assert(result == vk::Result::eSuccess);
    }
}
