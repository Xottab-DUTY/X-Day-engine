////////////////////////////////
//
//  Vulkan Renderer base
//  Author: Xottab_DUTY
//
////////////////////////////////
#include "pch.hpp"

#include <set>

#include <vulkan/vulkan.hpp>
#include <GLFW/glfw3.h>

#include "xdCore/Core.hpp"
#include "xdEngine/xdEngine.hpp"
#include "r_vulkan_base.hpp"
#include "r_vulkan_debug_callback.hpp"

bool enableValidationLayers = false;
bool validationLayersAvailable = true;

namespace XDay
{
namespace Renderer
{
const std::vector<const char*> validationLayers =
{
    "VK_LAYER_LUNARG_standard_validation"
};

const std::vector<const char*> deviceExtensions =
{
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

std::vector<const char*> r_vulkan_base::getValidationLayers()
{
    return validationLayers;
}

std::vector<const char*> r_vulkan_base::getDeviceExtensions()
{
    return deviceExtensions;
}

std::vector<const char*> r_vulkan_base::getRequiredExtensions()
{
    std::vector<const char*> extensions;

    uint32_t glfwExtensionCount = 0;
    const auto glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    for (uint32_t i = 0; i < glfwExtensionCount; ++i)
        extensions.emplace_back(glfwExtensions[i]);

    if (enableValidationLayers)
        extensions.emplace_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);

    return extensions;
}

bool r_vulkan_base::checkValidationLayersSupport() const
{
    auto availableLayers = vk::enumerateInstanceLayerProperties();

    for (std::string neededLayer : validationLayers)
    {
        bool found = false;

        for (const auto& layer : availableLayers)
        {
            if (neededLayer.compare(layer.layerName) == 0)
            {
                found = true;
                break;
            }
        }

        if (!found)
            return false;
    }

    return true;
}

void r_vulkan_base::Initialize()
{
    enableValidationLayers = Core.isGlobalDebug();
    createInstance();
    createDebugCallback();
}

void r_vulkan_base::createInstance()
{
    if (enableValidationLayers && !checkValidationLayersSupport())
    {
        Log::Error("Vulkan: not all validation layers supported.");
        validationLayersAvailable = false;
    }

    vk::ApplicationInfo appInfo(Core.GetAppName().c_str(), std::stoi(Core.GetAppVersion()),
                                Core.GetEngineName().c_str(), std::stoi(Core.GetEngineVersion()),
                                VK_API_VERSION_1_0);

    auto extensions = getRequiredExtensions();

    vk::InstanceCreateInfo i({}, &appInfo, 0, nullptr,
                             static_cast<uint32_t>(extensions.size()),
                             extensions.data());

    if (enableValidationLayers && validationLayersAvailable)
    {
        i.setEnabledLayerCount(static_cast<uint32_t>(validationLayers.size()));
        i.setPpEnabledLayerNames(validationLayers.data());
    }

    instance = createInstanceUnique(i);
    ASSERT1(instance);
}

void r_vulkan_base::createDebugCallback()
{
    if (!enableValidationLayers || !validationLayersAvailable) return;

    const vk::DebugReportCallbackCreateInfoEXT callbackInfo(
        {vk::DebugReportFlagBitsEXT::eError | vk::DebugReportFlagBitsEXT::eWarning},
        PFN_vkDebugReportCallbackEXT(vkDebugReportCallback));

    callback = instance->createDebugReportCallbackEXTUnique(callbackInfo);
    ASSERT1(callback);
}

void r_vulkan_base::createSurface()
{
    auto result = static_cast<vk::Result>(glfwCreateWindowSurface(
        reinterpret_cast<VkInstance&>(instance), Engine.windowMain,
        nullptr, reinterpret_cast<VkSurfaceKHR*>(&surface)));

    ASSERT1(result == vk::Result::eSuccess);
}

void r_vulkan_base::getPhysicalDevice()
{
    std::vector<vk::PhysicalDevice> physicalDevices = instance->enumeratePhysicalDevices();
    for (const auto& device : physicalDevices)
        if (isPhysicalDeviceSuitable(device, surface, deviceExtensions))
        {
            physicalDevice = device;
            break;
        }

    ASSERT1(physicalDevice);
}

void r_vulkan_base::createDevice()
{
    const auto indices = findQueueFamilies(physicalDevice, surface);

    std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;
    std::set<int> uniqueQueueFamilies = { indices.graphicsFamily, indices.presentFamily };

    float queue_priority = 1.0f;
    for (int queueFamily : uniqueQueueFamilies)
    {
        const vk::DeviceQueueCreateInfo queueCreateInfo({}, queueFamily, 1, &queue_priority);
        queueCreateInfos.emplace_back(queueCreateInfo);
    }

    vk::PhysicalDeviceFeatures deviceFeatures;
    deviceFeatures.setSamplerAnisotropy(VK_TRUE);

    vk::DeviceCreateInfo i{
        {},
        static_cast<uint32_t>(queueCreateInfos.size()), queueCreateInfos.data(),
        0, nullptr,
        static_cast<uint32_t>(deviceExtensions.size()), deviceExtensions.data(),
        &deviceFeatures
    };

    if (enableValidationLayers && validationLayersAvailable)
    {
        i.setEnabledLayerCount(static_cast<uint32_t>(validationLayers.size()));
        i.setPpEnabledLayerNames(validationLayers.data());
    }

    device = physicalDevice.createDeviceUnique(i);
    ASSERT1(device);

    //device->getQueue(indices.graphicsFamily, 0, &graphicsQueue);
    //device->getQueue(indices.presentFamily, 0, &presentQueue);
}
} // namespace Renderer
} // namespace XDay
