#pragma once
#ifndef xdRender_impexp_inl__
#define xdRender_impexp_inl__
#include "Common/import_export_macros.inl"

#ifdef RENDERER_EXPORTS
#define RENDERER_API XD_EXPORT
#else
#define RENDERER_API XD_IMPORT
#endif

#endif // xdRender_impexp_inl__
