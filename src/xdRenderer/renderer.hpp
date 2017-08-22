#pragma once

namespace XDay
{
namespace Renderer
{
struct XD_NOVTABLE Rocket
{
    virtual void launch() = 0;
    virtual void landing() = 0;

    virtual void orbit() = 0;
};
}
}
