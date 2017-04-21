#pragma once
#ifndef xdSound_impexp_inl__
#define xdSound_impexp_inl__
#include "Common/import_export_macros.inl"

#ifdef XDSOUND_EXPORTS
#define XDSOUND_API XD_EXPORT
#else
#define XDSOUND_API XD_IMPORT
#endif

#endif // xdSound_impexp_inl__
