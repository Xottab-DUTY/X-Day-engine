#pragma once
#ifndef Log_hpp__
#define Log_hpp__

#define SPDLOG_WCHAR_TO_UTF8_SUPPORT
#include <spdlog/spdlog.h>

#include "xdEngine_impexp.inl"

namespace XDay
{
class XDAY_API Logger
{
    bool nolog = false;
    bool nologflush = false;
    std::string LogFile = "X-Day.init.log";
    std::shared_ptr<spdlog::logger> spdlogger = nullptr;

public:
    Logger(const std::string& _logfile, bool coreInitialized);
    ~Logger();

    // Used only in GlobalLog
    void onCoreInitialized();

    void CloseLog();
    void FlushLog();

    bool isNoLog() const { return nolog; }
    bool isNoLogFlush() const { return nologflush; }

    template <typename... Args>
    void Log(std::string log, spdlog::level::level_enum level, const Args&... args)
    {
        if (nolog)
            return;

        spdlogger->log(level, log.c_str(), std::forward<Args>((Args)args)...);
    }

    template <typename... Args>
    void LogIf(bool flag, std::string log, spdlog::level::level_enum level, const Args&... args)
    {
        if (nolog)
            return;

        spdlogger->log_if(flag, level, log.c_str(), std::forward<Args>((Args)args)...);
    }

    template <typename... Args>
    inline void Trace(std::string trace, const Args&... args) { Log(trace, spdlog::level::trace, std::forward<Args>((Args)args)...); }
    template <typename... Args>
    inline void TraceIf(bool flag, std::string trace, const Args&... args) { Log(flag, trace, spdlog::level::trace, std::forward<Args>((Args)args)...); }

    template <typename... Args>
    inline void DebugMsg(std::string debug, const Args&... args) { Log(debug, spdlog::level::debug, std::forward<Args>((Args)args)...); }
    template <typename... Args>
    inline void DebugMsgIf(bool flag, std::string debug, const Args&... args) { Log(flag, debug, spdlog::level::debug, std::forward<Args>((Args)args)...); }

    template <typename... Args>
    inline void Info(std::string info, const Args&... args) { Log(info, spdlog::level::info, std::forward<Args>((Args)args)...); };
    template <typename... Args>
    inline void InfoIf(bool flag, std::string info, const Args&... args) { Log(flag, info, spdlog::level::info, std::forward<Args>((Args)args)...); };

    template <typename... Args>
    inline void Warning(std::string warning, const Args&... args) { Log(warning, spdlog::level::warn, std::forward<Args>((Args)args)...); }
    template <typename... Args>
    inline void WarningIf(bool flag, std::string warning, const Args&... args) { Log(flag, warning, spdlog::level::warn, std::forward<Args>((Args)args)...); }

    template <typename... Args>
    inline void Error(std::string error, const Args&... args) { Log(error, spdlog::level::err, std::forward<Args>((Args)args)...); };
    template <typename... Args>
    inline void ErrorIf(bool flag, std::string error, const Args&... args) { Log(flag, error, spdlog::level::err, std::forward<Args>((Args)args)...); };

    template <typename... Args>
    inline void Critical(std::string critical, const Args&... args) { Log(critical, spdlog::level::critical, std::forward<Args>((Args)args)...); }
    template <typename... Args>
    inline void CriticalIf(bool flag, std::string critical, const Args&... args) { Log(flag, critical, spdlog::level::critical, std::forward<Args>((Args)args)...); }

};
}

extern XDAY_API XDay::Logger GlobalLog;

template <typename... Args>
inline void Log(std::string log, spdlog::level::level_enum level, const Args&... args)
{
    GlobalLog.Log(log, level, std::forward<Args>((Args)args)...);
}
template <typename... Args>
inline void LogIf(bool flag, std::string log, spdlog::level::level_enum level, const Args&... args)
{
    GlobalLog.LogIf(flag, log, level, std::forward<Args>((Args)args)...);
}

template <typename... Args>
inline void Trace(std::string trace, const Args&... args) { Log(trace, spdlog::level::trace, std::forward<Args>((Args)args)...); }
template <typename... Args>
inline void TraceIf(bool flag, std::string trace, const Args&... args) { LogIf(flag, trace, spdlog::level::trace, std::forward<Args>((Args)args)...); }

template <typename... Args>
inline void DebugMsg(std::string debug, const Args&... args) { Log(debug, spdlog::level::debug, std::forward<Args>((Args)args)...); }
template <typename... Args>
inline void DebugMsgIf(bool flag, std::string debug, const Args&... args) { LogIf(flag, debug, spdlog::level::debug, std::forward<Args>((Args)args)...); }

template <typename... Args>
inline void Info(std::string info, const Args&... args) { Log(info, spdlog::level::info, std::forward<Args>((Args)args)...); };
template <typename... Args>
inline void InfoIf(bool flag, std::string info, const Args&... args) { LogIf(flag, info, spdlog::level::info, std::forward<Args>((Args)args)...); };

template <typename... Args>
inline void Warning(std::string warning, const Args&... args) { Log(warning, spdlog::level::warn, std::forward<Args>((Args)args)...); }
template <typename... Args>
inline void WarningIf(bool flag, std::string warning, const Args&... args) { LogIf(flag, warning, spdlog::level::warn, std::forward<Args>((Args)args)...); }

template <typename... Args>
inline void Error(std::string error, const Args&... args) { Log(error, spdlog::level::err, std::forward<Args>((Args)args)...); }
template <typename... Args>
inline void ErrorIf(bool flag, std::string error, const Args&... args) { LogIf(flag, error, spdlog::level::err, std::forward<Args>((Args)args)...); }

template <typename... Args>
inline void Critical(std::string critical, const Args&... args) { Log(critical, spdlog::level::critical, std::forward<Args>((Args)args)...); }
template <typename... Args>
inline void CriticalIf(bool flag, std::string critical, const Args&... args) { LogIf(flag, critical, spdlog::level::critical, std::forward<Args>((Args)args)...); }

inline void FlushLog() { GlobalLog.FlushLog(); }

#endif // Log_hpp__