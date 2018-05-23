#include "pch.hpp"
#include "Debug.hpp"

#include <new.h>
#include <signal.h>

#include "Console/ConsoleCommands.hpp"

namespace
{
void HandlerBase(cpcstr description)
{
    bool ignoreAlways = false;
    XDay::Debug::Fail(true, ignoreAlways, DEBUG_INFO, nullptr, description);
}

void PureCallHandler() { HandlerBase("Pure virtual function call"); }

void InvalidParameterHandler(const wchar_t* expression, const wchar_t* function, const wchar_t* file,
    unsigned int line, uintptr_t /*reserved*/)
{
    string4096 mbExpression;
    string4096 mbFunction;
    string4096 mbFile;
    size_t convertedChars = 0;

    if (expression)
        wcstombs_s(&convertedChars, mbExpression, sizeof(mbExpression), expression, (wcslen(expression) + 1) * 2);
    else
        strcpy_s(mbExpression, "");

    if (function)
        wcstombs_s(&convertedChars, mbFunction, sizeof(mbFunction), function, (wcslen(function) + 1) * 2);
    else
        strcpy_s(mbFunction, __FUNCTION__);

    if (file)
        wcstombs_s(&convertedChars, mbFile, sizeof(mbFile), file, (wcslen(file) + 1) * 2);
    else
    {
        line = __LINE__;
        strcpy_s(mbFile, __FILE__);
    }

    bool ignoreAlways = false;
    XDay::Debug::Fail(true, ignoreAlways, { mbFile, int(line), mbFunction }, mbExpression, "Invalid parameter");
}

int OutOfMemoryHandler(const size_t size)
{
    FATAL("Out of memory. Memory request: {} K", size / 1024);
    return 1;
}

void SetupHandlers()
{
    signal(SIGINT, nullptr);
    signal(SIGILL, [](int) { HandlerBase("Illegal instruction"); });
    signal(SIGFPE, [](int) { HandlerBase("Floating point error"); });
    signal(SIGSEGV, [](int) { HandlerBase("Segment violation"); });
    signal(SIGTERM, [](int) { HandlerBase("Termination with exit code 3"); });
    signal(SIGABRT, [](int) { HandlerBase("Application is aborting"); });
    signal(SIGABRT_COMPAT, [](int) { HandlerBase("Application is aborting"); });

    _set_abort_behavior(0, _WRITE_ABORT_MSG | _CALL_REPORTFAULT);
    _set_purecall_handler(&PureCallHandler);
    _set_invalid_parameter_handler(&InvalidParameterHandler);
    _set_new_mode(1);
    _set_new_handler(&OutOfMemoryHandler);
}
}

namespace XDay
{
void Debug::Initialize()
{
    SetupHandlers();
}

const vector<string> Debug::GetStackTrace()
{
    NOT_IMPLEMENTED();
    return {};
}

stringc Debug::FormatInfo(ErrorLocation&& location, cpcstr&& expression, cpcstr&& description)
{
    string buffer("\nFATAL ERROR\n\n");

    constexpr cpcstr prefix = "[error]";
    buffer += fmt::format("{} Expression    : {}\n", prefix, expression);
    buffer += fmt::format("{} Function      : {}\n", prefix, location.Function);
    buffer += fmt::format("{} File          : {}\n", prefix, location.File);
    buffer += fmt::format("{} Line          : {}\n", prefix, location.Line);

    if (description)
        buffer += fmt::format("{} Description   : {}\n", prefix, description);

    return buffer;
}

void Debug::Fail(const bool fatal, const bool& ignoreAlways, ErrorLocation&& location, pcstr expression, pcstr description)
{
    auto info = FormatInfo(std::move(location), std::move(expression), std::move(description));
    Log::Error(std::move(info));
    Log::Flush();

    if (ignoreAlways)
        return;

    if (fatal)
        Console::Terminate.Execute();
}
}