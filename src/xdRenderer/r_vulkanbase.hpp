////////////////////////////////
//
//  Vulkan Renderer base
//  Author: Xottab_DUTY
//
////////////////////////////////

#pragma once

#include "Common/Platform.hpp" // this must be first
#include <vulkan/vulkan.hpp>

#include "Common/import_export_macros.hpp"

namespace XDay
{
namespace Renderer
{
class XDRENDERER_API VulkanBase
{
public:
    vk::UniqueInstance instance;
    vk::UniqueDebugReportCallbackEXT callback;
    vk::UniqueSurfaceKHR surface;

    vk::PhysicalDevice physical_device;
    vk::UniqueDevice device;

//public:
    void create_instance();

//private:
    std::vector<const char*> get_validation_layers() const;
    std::vector<const char*> get_device_extensions() const;
    std::vector<const char*> get_required_extensions() const;
    bool check_validation_layers_support() const;
};
}
}