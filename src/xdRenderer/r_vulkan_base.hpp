////////////////////////////////
//
//  Vulkan Renderer base
//  Author: Xottab_DUTY
//
////////////////////////////////
#pragma once

#include <vulkan/vulkan.hpp>

#include "vulkan_helper.hpp"

namespace XDay
{
namespace Renderer
{
class XDRENDERER_API r_vulkan_base : public vulkan_helper
{
    vk::UniqueInstance instance;
    vk::UniqueDebugReportCallbackEXT callback;
    vk::UniqueSurfaceKHR surface;

    vk::PhysicalDevice physicalDevice;
    vk::UniqueDevice device;

public:
    virtual void Initialize();

    static std::vector<const char*> getValidationLayers();
    static std::vector<const char*> getDeviceExtensions();
    static std::vector<const char*> getRequiredExtensions();
    bool checkValidationLayersSupport() const;

private:
    void createInstance();
    void createDebugCallback();
    void createSurface();
    void getPhysicalDevice();
    void createDevice();
};
} // namespace Renderer
} // namespace XDay
