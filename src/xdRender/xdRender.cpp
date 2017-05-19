#include <vulkan/vulkan.hpp>
#include <GLFW/glfw3.h>

#include <fmt/ostream.h>

#include "xdEngine/Debug/Log.hpp"
#include "xdEngine/xdCore.hpp"
#include "xdEngine/xdEngine.hpp"
#include "xdRender.hpp"

RENDERER_API Renderer Render;

const bool enableValidationLayers = Core.isGlobalDebug();

const std::vector<const char*> validationLayers =
{
    "VK_LAYER_LUNARG_standard_validation"
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
    PFN_vkCreateInstance pfnCreateInstance = (PFN_vkCreateInstance)
        glfwGetInstanceProcAddress(nullptr, "vkCreateInstance");

    CreateVkInstance();
    if (enableValidationLayers && !CheckValidationLayersSupport())
        Log("Vulkan: not all validation layers supported.");

    

    PFN_vkCreateDevice pfnCreateDevice = (PFN_vkCreateDevice)
        glfwGetInstanceProcAddress(static_cast<VkInstance>(instance), "vkCreateDevice");
    
    GetPhysDevice();
    CreateDevice();
    CreateVkSurface();
    
}

void Renderer::CreateVkInstance()
{
    auto extensions = getRequiredExtensions();

    vk::ApplicationInfo appInfo(Core.AppName.c_str(), stoi(Core.AppVersion), Core.EngineName.c_str(), stoi(Core.EngineVersion), VK_MAKE_VERSION(1, 0, 42));
    vk::InstanceCreateInfo i({}, &appInfo,
        static_cast<uint32_t>(validationLayers.size()), validationLayers.data(),
        static_cast<uint32_t>(extensions.size()), extensions.data());

    result = vk::createInstance(&i, nullptr, &instance);
    assert(result == vk::Result::eSuccess);
}

bool Renderer::CheckValidationLayersSupport()
{
    uint32_t layerCount;
    vk::enumerateInstanceLayerProperties(&layerCount, nullptr);

    std::vector<vk::LayerProperties> availableLayers(layerCount);
    vk::enumerateInstanceLayerProperties(&layerCount, availableLayers.data());

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

static VKAPI_ATTR VkBool32 VKAPI_CALL cback()
{
    return true;
}

void Renderer::SetupDebugCallback()
{
    if (!enableValidationLayers) return;
    //vk::DebugReportCallbackCreateInfoEXT callbackInfo({vk::DebugReportFlagBitsEXT::eError | vk::DebugReportFlagBitsEXT::eWarning}, debugCallback, nullptr);

    /*VkDebugReportCallbackCreateInfoEXT createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
    createInfo.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT;
    createInfo.pfnCallback = debugCallback;*/

    VkDebugReportCallbackCreateInfoEXT createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
    createInfo.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT;
    createInfo.pfnCallback = cback;
}

std::vector<const char*> Renderer::getRequiredExtensions()
{
    std::vector<const char*> extensions;

    unsigned int glfwExtensionCount = 0;
    const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    for (unsigned int i = 0; i < glfwExtensionCount; i++)
    {
        extensions.push_back(glfwExtensions[i]);
    }

    if (enableValidationLayers)
        extensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);

    return extensions;
}

bool Renderer::isPhysDeviceSuitable(vk::PhysicalDevice _device)
{
    return true;
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

void Renderer::CreateDevice()
{
    vk::DeviceCreateInfo _createInfo;
    device = physDevice.createDevice(_createInfo);
}

void Renderer::Destroy() {}

void Renderer::CreateVkSurface() {}
