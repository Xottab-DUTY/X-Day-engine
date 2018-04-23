#pragma once

#define SPDLOG_WCHAR_TO_UTF8_SUPPORT
// XXX: Uncomment when it will be fixed
//#define SPDLOG_FMT_EXTERNAL
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
    template <typename... Args>
    static void Trace(std::string&& msg, Args&&... args)
    {
        Global.spdlogger->trace(msg.c_str(), std::forward<Args>(args)...);
    }

    template <typename... Args>
    static void Debug(std::string&& msg, Args&&... args)
    {
        Global.spdlogger->debug(msg.c_str(), std::forward<Args>(args)...);
    }

    template <typename... Args>
    static void Info(std::string&& msg, Args&&... args)
    {
        Global.spdlogger->info(msg.c_str(), std::forward<Args>(args)...);
    }

    template <typename... Args>
    static void Warning(std::string&& msg, Args&&... args)
    {
        Global.spdlogger->warn(msg.c_str(), std::forward<Args>(args)...);
    }

    template <typename... Args>
    static void Error(std::string&& msg, Args&&... args)
    {
        Global.spdlogger->error(msg.c_str(), std::forward<Args>(args)...);
    }

    template <typename... Args>
    static void Critical(std::string&& msg, Args&&... args)
    {
        Global.spdlogger->critical(msg.c_str(), std::forward<Args>(args)...);
    }
#pragma endregion Log functions
};
}
