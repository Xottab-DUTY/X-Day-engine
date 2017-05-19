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
    CreateVkInstance();
    if (enableValidationLayers && !CheckValidationLayersSupport())
        Log("Vulkan: not all validation layers supported.");

    GetPhysDevice();
    CreateDevice();
    CreateVkSurface();
    
}

void Renderer::Destroy()
{
    device.destroy();

    instance.destroy();
}

void Renderer::CreateVkInstance()
{
    auto extensions = getRequiredExtensions();

    vk::ApplicationInfo appInfo(Core.AppName.c_str(), stoi(Core.AppVersion),
        Core.EngineName.c_str(), stoi(Core.EngineVersion), 
        VK_MAKE_VERSION(1, 0, 42));

    vk::InstanceCreateInfo i;
    if (enableValidationLayers)
        i = vk::InstanceCreateInfo()
            .setFlags({})
            .setPApplicationInfo(&appInfo)
            .setEnabledLayerCount(static_cast<uint32_t>(validationLayers.size()))
            .setPpEnabledLayerNames(validationLayers.data())
            .setEnabledExtensionCount(static_cast<uint32_t>(extensions.size()))
            .setPpEnabledExtensionNames(extensions.data());
    else
        i = vk::InstanceCreateInfo()
            .setFlags({})
            .setPApplicationInfo(&appInfo)
            .setEnabledLayerCount(0)
            .setPpEnabledLayerNames(nullptr)
            .setEnabledExtensionCount(static_cast<uint32_t>(extensions.size()))
            .setPpEnabledExtensionNames(extensions.data());

    result = vk::createInstance(&i, nullptr, &instance);
    assert(result == vk::Result::eSuccess);
}

std::vector<const char*> Renderer::getRequiredExtensions()
{
    std::vector<const char*> extensions;

    unsigned int glfwExtensionCount = 0;
    const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    for (unsigned int i = 0; i < glfwExtensionCount; ++i)
    {
        extensions.push_back(glfwExtensions[i]);
    }

    if (enableValidationLayers)
        extensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);

    return extensions;
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
    /*vk::DebugReportCallbackCreateInfoEXT callbackInfo({vk::DebugReportFlagBitsEXT::eError | vk::DebugReportFlagBitsEXT::eWarning},
        debugCallback, nullptr);*/

}

struct QueueFamilyIndices
{
    int graphicsFamily = -1;

    bool isComplete() const
    {
        return graphicsFamily >= 0;
    }
};

QueueFamilyIndices findQueueFamilies(vk::PhysicalDevice device)
{
    QueueFamilyIndices indices;

    uint32_t queueFamilyCount = 0;
    device.getQueueFamilyProperties(&queueFamilyCount, nullptr);

    std::vector<vk::QueueFamilyProperties> queueFamilies(queueFamilyCount);
    device.getQueueFamilyProperties(&queueFamilyCount, queueFamilies.data());

    int i = 0;
    for (const auto& queueFamily : queueFamilies)
    {
        if (queueFamily.queueCount > 0 && queueFamily.queueFlags & vk::QueueFlagBits::eGraphics)
            indices.graphicsFamily = i;

        if (indices.isComplete())
            break;

        ++i;
    }

    return indices;
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

bool Renderer::isPhysDeviceSuitable(vk::PhysicalDevice _device)
{
    QueueFamilyIndices indices = findQueueFamilies(_device);

    return indices.isComplete();
}

void Renderer::CreateDevice()
{
    QueueFamilyIndices indices = findQueueFamilies(physDevice);

    float queuePriority = 1.0f;
    vk::DeviceQueueCreateInfo queueCreateInfo({}, indices.graphicsFamily, 1, &queuePriority);
    
    vk::PhysicalDeviceFeatures deviceFeatures = {};

    vk::DeviceCreateInfo deviceCreateInfo({}, 1, &queueCreateInfo);
    deviceCreateInfo.pEnabledFeatures = &deviceFeatures;
    deviceCreateInfo.enabledExtensionCount = 0;

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
}

void Renderer::CreateVkSurface()
{
    
}
