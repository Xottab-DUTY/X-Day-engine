#pragma once

#define SPDLOG_FMT_EXTERNAL
#include <spdlog/spdlog.h>

namespace XDay
{
class XDCORE_API Log
{
    bool noLog;
    bool noLogFlush;
    std::string logFile = "main.log";
    std::shared_ptr<spdlog::logger> spdlogger;

    Log();
    ~Log();
    static Log instance;

public:
    static void Initialize();

    static void Flush();

    static bool isNoLog();
    static bool isNoLogFlush();

#pragma region Log functions
    template <typename... Args>
    static void Trace(std::string&& msg, Args&&... args)
    {
        instance.spdlogger->trace(msg.c_str(), std::forward<Args>(args)...);
    }

    template <typename... Args>
    static void Debug(std::string&& msg, Args&&... args)
    {
        instance.spdlogger->debug(msg.c_str(), std::forward<Args>(args)...);
    }

    template <typename... Args>
    static void Info(std::string&& msg, Args&&... args)
    {
        instance.spdlogger->info(msg.c_str(), std::forward<Args>(args)...);
    }

    template <typename... Args>
    static void Warning(std::string&& msg, Args&&... args)
    {
        instance.spdlogger->warn(msg.c_str(), std::forward<Args>(args)...);
    }

    template <typename... Args>
    static void Error(std::string&& msg, Args&&... args)
    {
        instance.spdlogger->error(msg.c_str(), std::forward<Args>(args)...);
    }

    template <typename... Args>
    static void Critical(std::string&& msg, Args&&... args)
    {
        instance.spdlogger->critical(msg.c_str(), std::forward<Args>(args)...);
    }
#pragma endregion Log functions
};
}
