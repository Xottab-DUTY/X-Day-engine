#pragma once
#ifndef xdRender_impexp_inl__
#define xdRender_impexp_inl__
#include "common/import_export_macros.inl"

#ifdef XDRENDER_EXPORTS
#define XDRENDER_API XD_EXPORT
#else
#define XDRENDER_API XD_IMPORT
#endif

#endif // xdRender_impexp_inl__
