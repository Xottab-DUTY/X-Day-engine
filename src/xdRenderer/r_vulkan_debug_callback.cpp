////////////////////////////////
//
//  Vulkan debug callback
//  Author: Xottab_DUTY
//
////////////////////////////////

#include "Common/Platform.hpp" // this must be first
#include <vulkan/vulkan.hpp>
#include <GLFW/glfw3.h>

#include "r_vulkan_debug_callback.hpp"
#include "xdEngine/Debug/Log.hpp"

VKAPI_ATTR VkResult VKAPI_CALL vkCreateDebugReportCallbackEXT(VkInstance instance,
                                        const VkDebugReportCallbackCreateInfoEXT* pCreateInfo,
                                        const VkAllocationCallbacks* pAllocator,
                                        VkDebugReportCallbackEXT* pCallback)
{
    const auto func = PFN_vkCreateDebugReportCallbackEXT(glfwGetInstanceProcAddress(instance, "vkCreateDebugReportCallbackEXT"));
    if (func)
        return func(instance, pCreateInfo, pAllocator, pCallback);

    return VK_ERROR_EXTENSION_NOT_PRESENT;
}

VKAPI_ATTR void VKAPI_CALL vkDestroyDebugReportCallbackEXT(VkInstance instance,
                                     VkDebugReportCallbackEXT callback,
                                     const VkAllocationCallbacks* pAllocator)
{
    const auto func = PFN_vkDestroyDebugReportCallbackEXT(glfwGetInstanceProcAddress(instance, "vkDestroyDebugReportCallbackEXT"));
    if (func && callback)
        func(instance, callback, pAllocator);
}

VKAPI_ATTR vk::Bool32 VKAPI_CALL vkDebugReportCallback(vk::DebugReportFlagsEXT flags,
                                            vk::DebugReportObjectTypeEXT objType,
                                            uint64_t obj,
                                            size_t location,
                                            int32_t code,
                                            const char* layerPrefix,
                                            const char* msg,
                                            void* userData)
{
    Warning("\nValidation layer reports: \n" \
            "Flags: not working right now \n" \
            "Object type: {} \n" \
            "Object: {} \n" \
            "Location: {} \n" \
            "Code: {} \n" \
            "Layer prefix: {} \n" \
            "Message: {} \n" \
            "User data: {}\n",
            //vk::to_string(flags),
            vk::to_string(objType),
            obj, location,
            code, layerPrefix,
            msg, userData);

    return VK_FALSE;
}
