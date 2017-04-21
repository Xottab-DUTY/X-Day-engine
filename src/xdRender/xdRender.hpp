#pragma  once
#ifndef xdRender_hpp__
#define xdRender_hpp__
#include "xdRender_impexp.inl"

#include <vulkan/vulkan.hpp>
#include <GLFW/glfw3.h>

class RENDERER_API Renderer
{
public:
    VkApplicationInfo applicationInfo;
    VkInstanceCreateInfo instanceInfo;
    VkInstance instance;
    VkResult result;

    Renderer();
    void Initialize();
    void Destroy();
};

#endif // xdRender_hpp__

extern RENDERER_API Renderer Render;
