#include "Common/Platform.hpp"

#include "xdRenderer/renderer.hpp"

using namespace XDay;

namespace XDay
{
namespace Renderer
{
void Renderer::initialize()
{
    r_base::initialize();
}
XDRENDERER_API Renderer r_impl;
}
}

