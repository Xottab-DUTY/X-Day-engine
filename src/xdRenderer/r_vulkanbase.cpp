////////////////////////////////
//
//  Vulkan Renderer base
//  Author: Xottab_DUTY
//
////////////////////////////////

#include "Common/Platform.hpp" // this must be first
#include <vulkan/vulkan.hpp>

#include <GLFW/glfw3.h>

#include "xdEngine/Debug/Log.hpp"
#include "xdEngine/xdCore.hpp"
#include "r_vulkanbase.hpp"

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

const bool enableValidationLayers = Core.isGlobalDebug();

namespace XDay
{
namespace Renderer
{
std::vector<const char*> VulkanBase::get_validation_layers() const
{
    std::vector<const char*> validation_layers =
    {
        "VK_LAYER_LUNARG_standard_validation"
    };
    return validation_layers;
}

std::vector<const char*> VulkanBase::get_device_extensions() const
{
    const std::vector<const char*> device_extensions =
    {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };
    return device_extensions;
}

std::vector<const char*> VulkanBase::get_required_extensions() const
{
    std::vector<const char*> extensions;

    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    for (uint32_t i = 0; i < glfwExtensionCount; ++i)
        extensions.push_back(glfwExtensions[i]);

    if (enableValidationLayers)
        extensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);

    return extensions;
}

bool VulkanBase::check_validation_layers_support() const
{
    auto available_layers = vk::enumerateInstanceLayerProperties();

    for (std::string needed_layer : get_validation_layers())
    {
        bool found = false;

        for (const auto& layer : available_layers)
            if (needed_layer.compare(layer.layerName))
            {
                found = true;
                break;
            }

        if (!found)
            return false;
    }

    return true;
}

void VulkanBase::create_instance()
{
    if (enableValidationLayers && !check_validation_layers_support())
        Warning("Vulkan: not all validation layers supported.");

    vk::ApplicationInfo appInfo(Core.AppName.c_str(), stoi(Core.AppVersion),
                                Core.EngineName.c_str(), stoi(Core.EngineVersion),
                                VK_API_VERSION_1_0);

    auto extensions = get_required_extensions();

    vk::InstanceCreateInfo i({}, &appInfo, 0, nullptr,
                             static_cast<uint32_t>(extensions.size()),
                             extensions.data());

    if (enableValidationLayers)
    {
        i.setEnabledLayerCount(static_cast<uint32_t>(get_validation_layers().size()));
        i.setPpEnabledLayerNames(get_validation_layers().data());
    }

    instance = vk::createInstanceUnique(i);
    assert(instance);
}
}
}


