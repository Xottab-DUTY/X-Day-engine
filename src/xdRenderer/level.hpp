#pragma once
#include "Common/Platform.hpp"

#include <vector>

namespace XDay
{
namespace level
{
struct XD_NOVTABLE chunk
{
    virtual void load() = 0;
    virtual void unload() = 0;

    virtual void fill() = 0;
    virtual void drain() = 0;

    virtual void terrainLoad() = 0;
    virtual void terrainUnload() = 0;

    virtual void terrainLoadFlora() = 0;
    virtual void terrainUnloadFlora() = 0;

    virtual void visualsLoad() = 0;
    virtual void visualsUnload() = 0;

    virtual void visualAdd() = 0;
    virtual void visualRemove() = 0;

    virtual void lightsLoad() = 0;
    virtual void lightsUnload() = 0;

    virtual void lightAdd() = 0;
    virtual void lightRemove() = 0;
};
struct XD_NOVTABLE level
{
    std::vector<chunk> chunkList;

    virtual void load() = 0;
    virtual void unload() = 0;

    virtual void chunksLoad() = 0;
    virtual void chunksUnload() = 0;
};
}
}
