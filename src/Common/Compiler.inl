#if !defined(__GNUC__) && !defined(_MSC_VER)
#error Unsupported compiler
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
