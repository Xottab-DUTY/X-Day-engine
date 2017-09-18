////////////////////////////////
//
//  Vulkan debug callback
//  Author: Xottab_DUTY
//
////////////////////////////////

#pragma once
#include "Common/Platform.hpp" // this must be first
#include <vulkan/vulkan.hpp>

VKAPI_ATTR VkResult VKAPI_CALL vkCreateDebugReportCallbackEXT(VkInstance instance,
                                                              const VkDebugReportCallbackCreateInfoEXT* pCreateInfo,
                                                              const VkAllocationCallbacks* pAllocator,
                                                              VkDebugReportCallbackEXT* pCallback);

VKAPI_ATTR void VKAPI_CALL vkDestroyDebugReportCallbackEXT(VkInstance instance,
                                                           VkDebugReportCallbackEXT callback,
                                                           const VkAllocationCallbacks* pAllocator = nullptr);

VKAPI_ATTR vk::Bool32 VKAPI_CALL vkDebugReportCallback(
    vk::DebugReportFlagsEXT flags,
    vk::DebugReportObjectTypeEXT objType,
    uint64_t obj,
    size_t location,
    int32_t code,
    const char* layerPrefix,
    const char* msg,
    void* userData);
