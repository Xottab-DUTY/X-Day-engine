#include <vulkan/vulkan.hpp>
#include <GLFW/glfw3.h>

#include "xdEngine/xdCore.hpp"
#include "xdEngine/Console.hpp"
#include "xdRender.hpp"

RENDERER_API Renderer Render;

Renderer::Renderer()
{
    applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    applicationInfo.pNext = NULL;
    applicationInfo.pApplicationName = Core.AppName.c_str();
    applicationInfo.pEngineName = Core.EngineName.c_str();
    applicationInfo.engineVersion = std::stoi(Core.EngineVersion); // Engine development version
    applicationInfo.apiVersion = VK_API_VERSION_1_0;

    instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instanceInfo.pNext = NULL;
    instanceInfo.flags = 0; // flags, 0 for now
    instanceInfo.pApplicationInfo = &applicationInfo;
    instanceInfo.enabledLayerCount = 0; // disable layers
    instanceInfo.ppEnabledLayerNames = NULL;
    instanceInfo.enabledExtensionCount = 0; // disable extensions
    instanceInfo.ppEnabledExtensionNames = NULL;
}

void Renderer::Initialize()
{
    PFN_vkCreateInstance pfnCreateInstance = (PFN_vkCreateInstance)
        glfwGetInstanceProcAddress(nullptr, "vkCreateInstance");
    

    

    //PFN_vkCreateDevice pfnCreateDevice = (PFN_vkCreateDevice)
    //    glfwGetInstanceProcAddress(instance, "vkCreateDevice");
}

void Renderer::Destroy()
{

}
