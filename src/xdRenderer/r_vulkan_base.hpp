////////////////////////////////
//
//  Vulkan Renderer base
//  Author: Xottab_DUTY
//
////////////////////////////////
#pragma once

#include <vulkan/vulkan.hpp>

#include "Common/import_export_macros.hpp"

namespace XDay
{
namespace Renderer
{
class XDRENDERER_API r_vulkan_base
{
public:
    vk::UniqueInstance instance;
    vk::UniqueDebugReportCallbackEXT callback;
    vk::UniqueSurfaceKHR surface;

    vk::PhysicalDevice physical_device;
    vk::UniqueDevice device;

//public:
    virtual void initialize();

    void create_instance();
    void create_debug_callback();

//private:
    std::vector<const char*> get_validation_layers() const;
    std::vector<const char*> get_device_extensions() const;
    std::vector<const char*> get_required_extensions() const;
    bool check_validation_layers_support() const;
};
}
}