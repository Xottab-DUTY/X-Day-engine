#pragma once
#ifndef xdEngine_impexp_inl__
#define xdEngine_impexp_inl__

#include "common/import_export_macros.inl"

#ifdef XDAY_EXPORTS
#define XDAY_API XD_EXPORT
#else
#define XDAY_API XD_IMPORT
#endif
#endif // xdEngine_impexp_inl__
