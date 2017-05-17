#include <map>

#include <vulkan/vulkan.hpp>
#include <GLFW/glfw3.h>

#include "xdEngine/xdCore.hpp"
#include "xdEngine/Console.hpp"
#include "xdRender.hpp"
#include "xdEngine/xdEngine.hpp"


RENDERER_API Renderer Render;

Renderer::Renderer()
{

}

void Renderer::Initialize()
{
    PFN_vkCreateInstance pfnCreateInstance = (PFN_vkCreateInstance)
        glfwGetInstanceProcAddress(nullptr, "vkCreateInstance");

    CreateVkInstance();

    PFN_vkCreateDevice pfnCreateDevice = (PFN_vkCreateDevice)
        glfwGetInstanceProcAddress(static_cast<VkInstance>(instance), "vkCreateDevice");

    GetPhysDevice();
    CreateDevice();

    CreateVkSurface();
    
}

void Renderer::CreateVkInstance()
{
    vk::ApplicationInfo appInfo(Core.AppName.c_str(), 1, Core.EngineName.c_str(), stoi(Core.EngineVersion), VK_API_VERSION_1_0);
    vk::InstanceCreateInfo i({}, &appInfo, 0, nullptr, 0, nullptr);
    result = createInstance(&i, nullptr, &instance);
    assert(result == vk::Result::eSuccess);
}

void Renderer::CreateVkSurface()
{
    
}

void Renderer::CreateDevice()
{
    vk::DeviceCreateInfo _createInfo;
    device = physDevice.createDevice(_createInfo);
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

void Renderer::Destroy()
{

}

bool Renderer::isPhysDeviceSuitable(vk::PhysicalDevice _device)
{
    return true;
}
