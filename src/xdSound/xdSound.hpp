#pragma  once

#include "xdEngine/Platform/Platform.hpp"

#ifdef XDSOUND_EXPORTS
#define XDSOUND_API XD_EXPORT
#else
#define XDSOUND_API XD_IMPORT
#endif
