#ifndef WIN32
#define WIN32
#endif

#define VC_EXTRALEAN // Exclude rarely-used stuff from Windows headers
#define WIN32_LEAN_AND_MEAN // Exclude rarely-used stuff from Windows headers

#define NOGDICAPMASKS
//#define NOSYSMETRICS
#define NOMENUS
#define NOICONS
#define NOKEYSTATES
#define NODRAWTEXT
#define NOMEMMGR
#define NOMETAFILE
#define NOSERVICE
#define NOCOMM
#define NOHELP
#define NOPROFILER
#define NOMCX
#define NOMINMAX
#define DOSWIN32
#define _WIN32_DCOM

#pragma warning(push)
#pragma warning(disable:4005) // macro redefinition
#include <windows.h>
#include <windowsx.h>
#pragma warning(pop)
