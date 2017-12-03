#pragma once

#define SPDLOG_WCHAR_TO_UTF8_SUPPORT
#define SPDLOG_FMT_EXTERNAL
#include <spdlog/spdlog.h>

namespace XDay
{
class XDCORE_API Log
{
    bool noLog = false;
    bool noLogFlush = false;
    std::string logFile = "main.log";
    std::shared_ptr<spdlog::logger> spdlogger;

    explicit Log(bool coreInitialized = false);
    ~Log();
    static Log Global;

    void CloseLog() const;

public:
    static void Flush();

    static bool isNoLog();
    static bool isNoLogFlush();

    static void onCoreInitialized();

#pragma region Log functions
#pragma region Log::Trace
    template <typename... Args>
    static void Trace(std::string&& msg, Args&&... args)
    {
        Global.spdlogger->trace(msg.c_str(), std::forward<Args>(args)...);
    }

    template <typename... Args>
    static void TraceIf(const bool flag, std::string&& msg, Args&&... args)
    {
        Global.spdlogger->trace_if(flag, msg.c_str(), std::forward<Args>(args)...);
    }
#pragma endregion Log::Trace

#pragma region Log::Debug
    template <typename... Args>
    static void Debug(std::string&& msg, Args&&... args)
    {
        Global.spdlogger->debug(msg.c_str(), std::forward<Args>(args)...);
    }

    template <typename... Args>
    static void DebugIf(const bool flag, std::string&& msg, Args&&... args)
    {
        Global.spdlogger->debug_if(flag, msg.c_str(), std::forward<Args>(args)...);
    }
#pragma endregion Log::Debug

#pragma region Log::Info
    template <typename... Args>
    static void Info(std::string&& msg, Args&&... args)
    {
        Global.spdlogger->info(msg.c_str(), std::forward<Args>(args)...);
    }

    template <typename... Args>
    static void InfoIf(const bool flag, std::string&& msg, Args&&... args)
    {
        Global.spdlogger->info_if(flag, msg.c_str(), std::forward<Args>(args)...);
    }
#pragma endregion Log::Info

#pragma region Log::Warning
    template <typename... Args>
    static void Warning(std::string&& msg, Args&&... args)
    {
        Global.spdlogger->warn(msg.c_str(), std::forward<Args>(args)...);
    }

    template <typename... Args>
    static void WarningIf(const bool flag, std::string&& msg, Args&&... args)
    {
        Global.spdlogger->warn_if(flag, msg.c_str(), std::forward<Args>(args)...);
    }
#pragma endregion Log::Warning

#pragma region Log::Error
    template <typename... Args>
    static void Error(std::string&& msg, Args&&... args)
    {
        Global.spdlogger->error(msg.c_str(), std::forward<Args>(args)...);
    }

    template <typename... Args>
    static void ErrorIf(const bool flag, std::string&& msg, Args&&... args)
    {
        Global.spdlogger->error_if(flag, msg.c_str(), std::forward<Args>(args)...);
    }
#pragma endregion Log::Error

#pragma region Log::Critical
    template <typename... Args>
    static void Critical(std::string&& msg, Args&&... args)
    {
        Global.spdlogger->critical(msg.c_str(), std::forward<Args>(args)...);
    }

    template <typename... Args>
    static void CriticalIf(const bool flag, std::string&& msg, Args&&... args)
    {
        Global.spdlogger->critical_if(flag, msg.c_str(), std::forward<Args>(args)...);
    }
#pragma endregion Log::Critical
#pragma endregion Log functions
};
}
