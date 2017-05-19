#include <set>
#include <vulkan/vulkan.hpp>

#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
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

static VKAPI_ATTR vk::Bool32 VKAPI_CALL debugCallback(
    vk::DebugReportFlagsEXT flags,
    vk::DebugReportObjectTypeEXT objType,
    uint64_t obj,
    size_t location,
    int32_t code,
    const char* layerPrefix,
    const char* msg,
    void* userData)
{

    Msg("Validation layer reports: \n" \
        "Object: {} \n" \
        "Code: {} \n" \
        "Layer prefix: {} \n" \
        "Message: {} \n" \
        "User data: {}",
        obj, location,
        code, layerPrefix,
        msg, userData);

    return VK_FALSE;
}

Renderer::Renderer(): result(vk::Result::eNotReady) {}

void Renderer::Initialize()
{
    CreateVkInstance();
    if (enableValidationLayers && !CheckValidationLayersSupport())
        Log("Vulkan: not all validation layers supported.");

    CreateVkSurface();
    GetPhysDevice();
    CreateDevice();
}

void Renderer::Destroy()
{
    device.destroy();
    instance.destroySurfaceKHR(surface);
    instance.destroy();
}

void Renderer::CreateVkInstance()
{
    uint32_t glfwExtensionCount = 0;
    auto glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    vk::ApplicationInfo appInfo(Core.AppName.c_str(), stoi(Core.AppVersion),
                                Core.EngineName.c_str(), stoi(Core.EngineVersion), 
                                VK_MAKE_VERSION(1, 0, 42));

    vk::InstanceCreateInfo i;
    i.setPApplicationInfo(&appInfo);
    i.setEnabledExtensionCount(glfwExtensionCount);
    i.setPpEnabledExtensionNames(glfwExtensions);

    if (enableValidationLayers)
    {
        i.setEnabledLayerCount(static_cast<uint32_t>(validationLayers.size()));
        i.setPpEnabledLayerNames(validationLayers.data());
    }

    result = vk::createInstance(&i, nullptr, &instance);
    assert(result == vk::Result::eSuccess);
}

bool Renderer::CheckValidationLayersSupport()
{
    std::vector<vk::LayerProperties> availableLayers = vk::enumerateInstanceLayerProperties();

    for (std::string layerName : validationLayers)
    {
        bool layerFound = false;

        for (const auto& layerProperties : availableLayers)
        {
            if (layerName.compare(layerProperties.layerName))
            {
                layerFound = true;
                break;
            }
        }
        if (!layerFound)
            return false;
    }

    return true;
}

void Renderer::SetupDebugCallback()
{
    if (!enableValidationLayers) return;
    /*vk::DebugReportCallbackCreateInfoEXT callbackInfo({vk::DebugReportFlagBitsEXT::eError | vk::DebugReportFlagBitsEXT::eWarning},
        debugCallback, nullptr);*/

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

        if (queueFamily.queueCount > 0 && presentSupport) {
            indices.presentFamily = i;
        }

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

    uint32_t presentModeCount;
    _physDevice.getSurfacePresentModesKHR(surface, &presentModeCount, nullptr);

    if (presentModeCount != 0)
    {
        details.presentModes.resize(presentModeCount);
        _physDevice.getSurfacePresentModesKHR(surface, &presentModeCount, details.presentModes.data());
    }

    return details;
}

void Renderer::GetPhysDevice()
{
    std::vector<vk::PhysicalDevice> physDevices = instance.enumeratePhysicalDevices();
    for (const auto& _device : physDevices)
    {
        if (isPhysDeviceSuitable(_device))
        {
            physDevice = _device;
            break;
        }
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
    {
        requiredExtensions.erase(extension.extensionName);
    }

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
    deviceCreateInfo.enabledExtensionCount = deviceExtensions.size();
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

void Renderer::CreateVkSurface()
{
    vk::Win32SurfaceCreateInfoKHR surfaceInfo({}, nullptr, glfwGetWin32Window(Engine.window));
    
    instance.createWin32SurfaceKHR(&surfaceInfo, nullptr, &surface);
    assert(result == vk::Result::eSuccess);
}
