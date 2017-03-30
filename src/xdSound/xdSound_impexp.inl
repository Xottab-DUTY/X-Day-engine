#include "common/import_export_macros.inl"

#ifdef XDSOUND_EXPORTS
#define XDSOUND_API XD_EXPORT
#else
#define XDSOUND_API XD_IMPORT
#endif