#pragma once
#ifndef import_export_macros_inl__
#define import_export_macros_inl__

#if defined(__GNUC__)
#define XD_EXPORT __attribute__ ((visibility("default")))
#define XD_IMPORT __attribute__ ((visibility("default")))
#elif defined(_MSC_VER)
#define XD_EXPORT __declspec(dllexport)
#define XD_IMPORT __declspec(dllimport)
#endif

#endif // import_export_macros_inl__
