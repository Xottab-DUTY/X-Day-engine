#pragma once

#include "Common/import_export_macros.hpp"

namespace XDay
{
namespace Renderer
{
class Renderer;
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
