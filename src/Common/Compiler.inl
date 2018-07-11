#if defined(_MSC_VER)
#pragma message("Compiling with MSVC")
#elif defined(__GNUC__)
#pragma message("Compiling with GCC")
#else
#error Unsupported compiler
#endif

#if defined(__GNUC__)
#define XD_EXPORT __attribute__ ((visibility("default")))
#define XD_IMPORT __attribute__ ((visibility("default")))
#elif defined(_MSC_VER)
#define XD_EXPORT __declspec(dllexport)
#define XD_IMPORT __declspec(dllimport)
#endif

#if defined(_M_X64) || defined(_WIN64) ||\
    defined(_M_AMD64) || defined(__amd64__) ||\
    defined(_M_IA64) || defined(__ia64__)
#define XR_X64
#pragma message("Compiling with x64")
#elif defined(_M_X86) || defined(_M_IX86) ||\
     (defined(_WIN32) && !defined(_WIN64))
#define XR_X86
#pragma message("Compiling with x86")
#else
#pragma message("Compiling with unknown architecture")
#endif

#if defined(__GNUC__)
#define XD_ASSUME(expr)  if (expr){} else __builtin_unreachable()
#elif defined(_MSC_VER)
#define XD_ASSUME(expr) __assume(expr)
#endif

#define UNUSED(...) (void)(__VA_ARGS__)

#if defined(_MSC_VER)
#define XD_NOVTABLE __declspec(novtable)
#else
#define XD_NOVTABLE
#endif
