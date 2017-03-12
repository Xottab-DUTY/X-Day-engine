#pragma once

#include "Platform/Platform.hpp"

#ifdef XDENGINE_IMPORTS
#define XDENGINE_API XD_IMPORT
#else
#define XDENGINE_API XD_EXPORT
#endif

class XDayEngine
{

};

extern XDENGINE_API XDayEngine Engine;