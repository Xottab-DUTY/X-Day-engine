#pragma once

namespace XDay
{
namespace Renderer
{
class XDRENDERER_API Renderer;
}

namespace Global
{
class XDAPI_API Environment
{
public:
    Renderer::Renderer* Render;
};
}
extern XDAPI_API Global::Environment GEnv;
}
