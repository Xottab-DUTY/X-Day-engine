
#include <spdlog/sinks/stdout_sinks.h>
#include <spdlog/sinks/msvc_sink.h>

#include "xdCore.hpp"
#include "Log.hpp"

using namespace XDay;

XDAY_API Logger GlobalLog("init.log", false);

Logger::Logger(const std::string& _logfile, bool coreInitialized) : LogFile(_logfile)
{
    if (Core.FindParam(eParamNoLog))
    {
        nolog = true;
        return;
    }
        
    if (Core.FindParam(eParamNoLogFlush))
        nologflush = true;

    std::vector<spdlog::sink_ptr> sinks;
    sinks.push_back(std::make_shared<spdlog::sinks::stdout_sink_mt>());

    if (!nologflush && coreInitialized)
        sinks.push_back(std::make_shared<spdlog::sinks::simple_file_sink_mt>(Core.LogsPath.string() + LogFile, true));

#if defined(DEBUG) && defined(_MSC_VER)
    sinks.push_back(std::make_shared<spdlog::sinks::msvc_sink_mt>());
#endif

    spdlogger = std::make_shared<spdlog::logger>("X-Day Engine", begin(sinks), end(sinks));
    spdlogger->set_pattern("[%T] [%l] %v");
    spdlog::register_logger(spdlogger);
}

Logger::~Logger()
{
    if (nolog)
        return;

    CloseLog();
}

// Used only in GlobalLog
void Logger::onCoreInitialized()
{
    if (nolog || nologflush)
        return;

    CloseLog();
    this->Logger::Logger("main.log", true);
}

void Logger::CloseLog()
{
    if (nolog)
        return;

    if (!nologflush)
        FlushLog();

    spdlog::drop_all();
}

void Logger::FlushLog()
{
    if (nologflush)
        return;

    spdlogger->flush();
}
