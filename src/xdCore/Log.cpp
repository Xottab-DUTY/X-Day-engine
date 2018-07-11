#include "pch.hpp"

#include <spdlog/sinks/stdout_color_sinks.h>
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

constexpr cpcstr LOGGER_NAME = "X-Day Engine";

Log Log::instance;

Log::Log()
{
    noLog = false;
    noLogFlush = false;
    vector<spdlog::sink_ptr> sinks;
    sinks.emplace_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
#ifdef WINDOWS
    sinks.emplace_back(std::make_shared<spdlog::sinks::msvc_sink_mt>());
#endif

    spdlogger = std::make_shared<spdlog::logger>(LOGGER_NAME, begin(sinks), end(sinks));
    spdlogger->set_pattern("%^[%T][%l] %v%$");
    spdlogger->set_level(spdlog::level::trace);
    spdlog::register_logger(spdlogger);
}

Log::~Log()
{
    noLog = true;
    instance.spdlogger->set_level(spdlog::level::off);

    Flush();
    noLogFlush = true;

    spdlog::drop_all();
}

void Log::Initialize()
{
    if (CommandLine::KeyNoLog.IsSet())
    {
        instance.noLog = true;
        instance.noLogFlush = true;
        instance.spdlogger->set_level(spdlog::level::off);
        return;
    }

    if (!Core.isGlobalDebug())
        instance.spdlogger->set_level(spdlog::level::info);

    if (CommandLine::KeyNoLogFlush.IsSet())
    {
        instance.noLogFlush = true;
        return;
    }

    auto sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(FS.LogsPath.string() + instance.logFile, true);
    sink->set_pattern("%^[%T][%l] %v%$"); // we need to set the pattern again for new sink

    // replace the hack below if the issue would be solved
    // https://github.com/gabime/spdlog/issues/674
    auto& sinks = const_cast<std::vector<spdlog::sink_ptr>&>(instance.spdlogger->sinks());
    sinks.emplace_back(std::move(sink));
}

void Log::Flush()
{
    if (instance.noLogFlush)
        return;

    spdlog::get(LOGGER_NAME)->flush();
}

bool Log::isNoLog()
{
    return instance.noLog;
}

bool Log::isNoLogFlush()
{
    return instance.noLogFlush;
}
} // namespace XDay
