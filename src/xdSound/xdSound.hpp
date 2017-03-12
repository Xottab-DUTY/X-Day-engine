#pragma  once

#include "xdEngine/Platform/Platform.hpp"

#ifdef XDSOUND_IMPORTS
#define XDSOUND_API XD_IMPORT
#else
#define XDSOUND_API XD_EXPORT
#endif
