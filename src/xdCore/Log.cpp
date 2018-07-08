#include "pch.hpp"

#include <spdlog/sinks/stdout_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/msvc_sink.h>

#include "Core.hpp"
#include "CommandLine.hpp"
#include "Filesystem.hpp"
#include "Log.hpp"

namespace XDay
{
namespace CommandLine
{
Key KeyNoLog("nolog", "Disables engine log", KeyType::Boolean);
Key KeyNoLogFlush("nologflush", "Disables log flushing", KeyType::Boolean);
}

Log Log::instance;

void Log::Initialize()
{
    if (CommandLine::KeyNoLog.IsSet())
    {
        instance.noLog = true;
        return;
    }

    if (CommandLine::KeyNoLogFlush.IsSet())
        instance.noLogFlush = true;

    std::vector<spdlog::sink_ptr> sinks;
    sinks.emplace_back(std::make_shared<spdlog::sinks::stdout_sink_mt>());

    if (!instance.noLogFlush)
        sinks.emplace_back(std::make_shared<spdlog::sinks::basic_file_sink_mt>(FS.LogsPath.string() + instance.logFile, true));

#ifdef WINDOWS
    sinks.emplace_back(std::make_shared<spdlog::sinks::msvc_sink_mt>());
#endif

    instance.spdlogger = std::make_shared<spdlog::logger>("X-Day Engine", begin(sinks), end(sinks));
    instance.spdlogger->set_pattern("[%T] [%l] %v");

    if (Core.isGlobalDebug())
        instance.spdlogger->set_level(spdlog::level::trace);

    spdlog::register_logger(instance.spdlogger);
}

void Log::Destroy()
{
    if (!instance.noLogFlush)
        Flush();

    if (!instance.noLog)
        spdlog::drop(instance.spdlogger->name());
}

void Log::Flush()
{
    if (instance.noLogFlush)
        return;

    spdlog::get("X-Day Engine")->flush();
}

bool Log::isNoLog()
{
    return instance.noLog;
}

bool Log::isNoLogFlush()
{
    return instance.noLogFlush;
}

void Log::onCoreInitialized()
{
    if (instance.noLog || instance.noLogFlush)
        return;

    instance.CloseLog();
    instance.Log::Log();
}

void Log::CloseLog() const
{
    if (noLog)
        return;

    if (!noLogFlush)
        Flush();

    spdlog::drop_all();
}
} // namespace XDay
