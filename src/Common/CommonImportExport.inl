#pragma once

#ifdef XDAY_EXPORTS
#define XDENGINE_API XD_EXPORT
#else
#define XDENGINE_API XD_IMPORT
#endif

#ifdef XDAPI_EXPORTS
#define XDAPI_API XD_EXPORT
#else
#define XDAPI_API XD_IMPORT
#endif

#ifdef XDCORE_EXPORTS
#define XDCORE_API XD_EXPORT
#else
#define XDCORE_API XD_IMPORT
#endif

#ifdef XDRENDERER_EXPORTS
#define XDRENDERER_API XD_EXPORT
#else
#define XDRENDERER_API XD_IMPORT
#endif
