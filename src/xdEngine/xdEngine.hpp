#pragma once

#include "Platform/Platform.hpp"

#ifdef XDENGINE_EXPORTS
#define XDENGINE_API XD_EXPORT
#else
#define XDENGINE_API XD_IMPORT
#endif

class XDENGINE_API XDayEngine
{

};

extern XDENGINE_API XDayEngine Engine;
