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

#define NODEFAULT() FATAL("No default in {} reached", __FUNCTION__)
#define NOT_IMPLEMENTED() VERIFY(false, "Function {} is not implemented", __FUNCTION__)