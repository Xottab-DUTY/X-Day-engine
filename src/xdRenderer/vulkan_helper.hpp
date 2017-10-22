#pragma once

#include "vulkan/vulkan.hpp"

namespace XDay
{
namespace Renderer
{
class vulkan_helper
{
public:
    struct QueueFamilyIndices
    {
        int graphicsFamily = -1;
        int presentFamily = -1;

        bool isComplete() const;
    };

    struct SwapChainSupportDetails
    {
        vk::SurfaceCapabilitiesKHR capabilities;
        std::vector<vk::SurfaceFormatKHR> formats;
        std::vector<vk::PresentModeKHR> presentModes;
    };

    static QueueFamilyIndices findQueueFamilies(const vk::PhysicalDevice& physical_device,
                                           const vk::UniqueSurfaceKHR& surface);

    static bool checkDeviceExtensionSupport(const vk::PhysicalDevice& physical_device,
                                        const std::vector<const char*>& device_extensions);

    static bool isPhysicalDeviceSuitable(const vk::PhysicalDevice& physical_device, const vk::UniqueSurfaceKHR& surface,
                                     const std::vector<const char*>& device_extensions);

    static SwapChainSupportDetails querySwapchainSupport(const vk::PhysicalDevice& physical_device,
                                                    const vk::UniqueSurfaceKHR& surface);
};
} // namespace Renderer
} // namespace XDay
