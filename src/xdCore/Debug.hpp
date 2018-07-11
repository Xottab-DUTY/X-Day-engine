#pragma once

#define DEBUG_INFO { __FILE__, __LINE__, __FUNCTION__ }

#define VERIFY(expression, description, ...) do\
    {\
        static bool ignoreAlways = false;\
        if (!ignoreAlways && !(expression))\
            XDay::Debug::Fail(false, ignoreAlways, DEBUG_INFO, #expression, fmt::format(description, __VA_ARGS__).c_str());\
    } while (false);

#define VERIFY1(expression) do\
    {\
        static bool ignoreAlways = false;\
        if (!ignoreAlways && !(expression))\
            XDay::Debug::Fail(false, ignoreAlways, DEBUG_INFO, #expression, nullptr);\
    } while (false);

#define VERIFY2(expression, description) do\
    {\
        static bool ignoreAlways = false;\
        if (!ignoreAlways && !(expression))\
            XDay::Debug::Fail(false, ignoreAlways, DEBUG_INFO, #expression, description);\
    } while (false);

#define ASSERT(expression, description, ...) do\
    {\
        static bool ignoreAlways = false;\
        if (!ignoreAlways && !(expression))\
            XDay::Debug::Fail(true, ignoreAlways, DEBUG_INFO, #expression, fmt::format(description, __VA_ARGS__).c_str());\
    } while (false);

#define ASSERT1(expression) do\
    {\
        static bool ignoreAlways = false;\
        if (!ignoreAlways && !(expression))\
            XDay::Debug::Fail(true, ignoreAlways, DEBUG_INFO, #expression, nullptr);\
    } while (false);

#define ASSERT2(expression, description) do\
    {\
        static bool ignoreAlways = false;\
        if (!ignoreAlways && !(expression))\
            XDay::Debug::Fail(true, ignoreAlways, DEBUG_INFO, #expression, description);\
    } while (false);

#define FATAL(description, ...) do\
    {\
        static bool ignoreAlways = true;\
        XDay::Debug::Fail(true, ignoreAlways, DEBUG_INFO, nullptr, fmt::format(description, __VA_ARGS__).c_str());\
    } while (false);

#define NODEFAULT() FATAL("No default reached in {}", __FUNCTION__)
#define NOT_IMPLEMENTED() VERIFY(false, "Function {} is not implemented", __FUNCTION__)

namespace XDay
{
struct ErrorLocation
{
    cpcstr File = nullptr;
    const int Line = -1;
    cpcstr Function = nullptr;
    ErrorLocation() = delete;
    constexpr ErrorLocation(cpcstr file, const int line, cpcstr function)
        : File(file), Line(line), Function(function) {}
};

class XDCORE_API Debug
{
    Debug();
    ~Debug();
    static Debug instance;

public:
    static const vector<string> GetStackTrace();

    static stringc FormatInfo(ErrorLocation location, pcstr expression, pcstr description);
    static void Fail(const bool fatal, const bool& ignoreAlways, ErrorLocation location, pcstr expression, pcstr description);
};
} // namespace XDay
