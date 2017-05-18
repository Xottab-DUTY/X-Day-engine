#include <vulkan/vulkan.hpp>
#include <GLFW/glfw3.h>

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

    vk::ApplicationInfo appInfo(Core.AppName.c_str(), 1, Core.EngineName.c_str(), stoi(Core.EngineVersion), VK_API_VERSION_1_0);
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
