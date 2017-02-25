#pragma once
#include "Platform.hpp"

#ifdef XDENGINE_IMPORTS
#define XDENGINE_API XD_IMPORT
#else
#define XDENGINE_API XD_EXPORT
#endif
