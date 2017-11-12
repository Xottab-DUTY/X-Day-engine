#include "pch.hpp"

#include <spdlog/sinks/stdout_sinks.h>
#include <spdlog/sinks/msvc_sink.h>

#include "xdCore.hpp"
#include "Log.hpp"

namespace XDay
{
Log Log::Global;
Log::Log(const bool coreInitialized /*= false*/)
{
    if (Core.FindParam(eParamNoLog))
    {
        noLog = true;
        return;
    }

    if (Core.FindParam(eParamNoLogFlush))
        noLogFlush = true;

    std::vector<spdlog::sink_ptr> sinks;
    sinks.emplace_back(std::make_shared<spdlog::sinks::stdout_sink_mt>());

    if (!noLogFlush && coreInitialized)
        sinks.emplace_back(std::make_shared<spdlog::sinks::simple_file_sink_mt>(Core.LogsPath.string() + logFile, true));

    if (Core.isGlobalDebug())
        sinks.emplace_back(std::make_shared<spdlog::sinks::msvc_sink_mt>());

    std::shared_ptr<spdlog::logger> spdlogger = std::make_shared<spdlog::logger>("X-Day Engine", begin(sinks), end(sinks));
    spdlogger->set_pattern("[%T] [%l] %v");

    if (Core.isGlobalDebug())
    spdlogger->set_level(spdlog::level::trace);

    spdlog::register_logger(spdlogger);
}

Log::~Log()
{
    if (!noLogFlush)
        Flush();

    spdlog::drop_all();
}

void Log::Flush()
{
    if (Global.noLogFlush)
        return;

    spdlog::get("X-Day Engine")->flush();
}

bool Log::isNoLog()
{
    return Global.noLog;
}

bool Log::isNoLogFlush()
{
    return Global.noLogFlush;
}

void Log::onCoreInitialized()
{
    if (Global.noLog || Global.noLogFlush)
        return;

    Global.CloseLog();
    Global.Log::Log(true);
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
