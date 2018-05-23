#pragma once

#include "DebugMacros.hpp"

namespace XDay
{
struct ErrorLocation
{
    cpcstr File = nullptr;
    const int Line = -1;
    cpcstr Function = nullptr;
};

class XDCORE_API Debug
{
    Debug() = delete;

public:
    static void Initialize();

    static const vector<string> GetStackTrace();

    static stringc FormatInfo(ErrorLocation&& location, cpcstr&& expression, cpcstr&& description);
    static void Fail(const bool fatal, const bool& ignoreAlways, ErrorLocation&& location, pcstr expression, pcstr description);
    };
} // namespace XDay
