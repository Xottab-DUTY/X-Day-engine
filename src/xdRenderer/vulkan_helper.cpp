#include "pch.hpp"

#include <set>
#include "vulkan/vulkan.hpp"

#include "vulkan_helper.hpp"

namespace XDay
{
namespace Renderer
{
bool vulkan_helper::QueueFamilyIndices::isComplete() const
{
    return graphicsFamily >= 0 && presentFamily >= 0;
}

vulkan_helper::QueueFamilyIndices vulkan_helper::findQueueFamilies(const vk::PhysicalDevice& physicalDevice,
                                                                     const vk::UniqueSurfaceKHR& surface)
{
    QueueFamilyIndices indices;

    std::vector<vk::QueueFamilyProperties> queueFamilies = physicalDevice.getQueueFamilyProperties();

    int i = 0;
    for (const auto& queueFamily : queueFamilies)
    {
        if (queueFamily.queueCount > 0 && queueFamily.queueFlags & vk::QueueFlagBits::eGraphics)
            indices.graphicsFamily = i;

        vk::Bool32 presentSupport = false;
        physicalDevice.getSurfaceSupportKHR(i, *surface, &presentSupport);

        if (queueFamily.queueCount > 0 && presentSupport)
            indices.presentFamily = i;

        if (indices.isComplete())
            break;

        ++i;
    }

    return indices;
}

bool vulkan_helper::checkDeviceExtensionSupport(const vk::PhysicalDevice& physicalDevice,
                                                   const std::vector<const char*>& deviceExtensions)
{
    auto availableExtensions = physicalDevice.enumerateDeviceExtensionProperties();

    std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

    for (const auto& extension : availableExtensions)
        requiredExtensions.erase(extension.extensionName);

    return requiredExtensions.empty();
}

bool vulkan_helper::isPhysicalDeviceSuitable(const vk::PhysicalDevice& physicalDevice,
                                                const vk::UniqueSurfaceKHR& surface,
                                                const std::vector<const char*>& deviceExtensions)
{
    auto indices = findQueueFamilies(physicalDevice, surface);

    const bool extensionsSupported = checkDeviceExtensionSupport(physicalDevice, deviceExtensions);

    bool swapChainAdequate = false;
    if (extensionsSupported)
    {
        SwapChainSupportDetails swapChainSupport = querySwapchainSupport(physicalDevice, surface);
        swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
    }

    const auto supportedFeatures = physicalDevice.getFeatures();

    return indices.isComplete() && extensionsSupported && swapChainAdequate && supportedFeatures.samplerAnisotropy;
}

vulkan_helper::SwapChainSupportDetails vulkan_helper::querySwapchainSupport(
    const vk::PhysicalDevice& physicalDevice, const vk::UniqueSurfaceKHR& surface)
{
    SwapChainSupportDetails details;

    physicalDevice.getSurfaceCapabilitiesKHR(*surface, &details.capabilities);

    {
        uint32_t formatCount;
        physicalDevice.getSurfaceFormatsKHR(*surface, &formatCount, nullptr);

        if (formatCount != 0)
        {
            details.formats.resize(static_cast<size_t>(formatCount));
            physicalDevice.getSurfaceFormatsKHR(*surface, &formatCount, details.formats.data());
        }
    }

    {
        uint32_t presentModeCount;
        physicalDevice.getSurfacePresentModesKHR(*surface, &presentModeCount, nullptr);

        if (presentModeCount != 0)
        {
            details.presentModes.resize(static_cast<size_t>(presentModeCount));
            physicalDevice.getSurfacePresentModesKHR(*surface, &presentModeCount, details.presentModes.data());
        }
    }

    return details;
}
} // namespace Renderer
} // namespace XDay
