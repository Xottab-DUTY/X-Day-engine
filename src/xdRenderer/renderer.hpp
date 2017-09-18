#pragma once

#include "xdAPI/xdAPI.hpp"
#include "r_vulkan_base.hpp"

namespace XDay
{
namespace Renderer
{
class XDRENDERER_API Renderer : r_vulkan_base
{
    using r_base = r_vulkan_base;

public:
    void initialize() override;
};
extern XDRENDERER_API Renderer r_impl;
}
}
