#pragma once

#if defined(__GNUC__)
#define XD_EXPORT __attribute__ ((visibility("default")))
#define XD_IMPORT __attribute__ ((visibility("default")))
#elif defined(_MSC_VER)
#define XD_EXPORT __declspec(dllexport)
#define XD_IMPORT __declspec(dllimport)
#endif

#ifdef XDAY_EXPORTS
#define XDAY_API XD_EXPORT
#else
#define XDAY_API XD_IMPORT
#endif

#ifdef XDRENDERER_EXPORTS
#define XDRENDERER_API XD_EXPORT
#else
#define XDRENDERER_API XD_IMPORT
#endif

#ifdef XDAPI_EXPORTS
#define XDAPI_API XD_EXPORT
#else
#define XDAPI_API XD_IMPORT
#endif
