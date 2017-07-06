#include <fstream>
#include <iostream>

#include "xdCore.hpp"
#include "Log.hpp"

using namespace XDay;

XDAY_API Logger* GlobalLog = nullptr;

Logger::Logger()
{
    LogContainer = new std::vector<std::string>();
    LogContainer->reserve(1000);
}



Logger::Logger(std::string&& _logfile) :  Logger()
{
    LogFile = _logfile;
}

// Used only in GlobalLog
void Logger::InitLog()
{
    if (!Core.LogsPath.empty())
        LogFile = Core.LogsPath.string() + "init.log";
    if (LogContainer) CloseLog();
    if (Core.FindParam("--p_nolog")) nolog = true;
    if (Core.FindParam("--p_nologflush")) nologflush = true;
    if (!nolog)
    {
        Core.FindParam("--p_mainlog") ? LogFile = Core.LogsPath.string() + Core.ReturnParam("--p_mainlog") : LogFile = Core.LogsPath.string() + "main.log";
        this->Logger::Logger();
    }
}

void Logger::CloseLog()
{
    if (LogContainer)
    {
        FlushLog();
        LogContainer->clear();
        delete LogContainer;
    }
}

void Logger::FlushLog()
{
    if (LogContainer)
    {
        if (!nologflush)
        {
            Log("Flushing log..");
            std::ofstream f(LogFile);
            for (auto&& str : *LogContainer)
            {
                f << str << std::endl;
            }
            f.close();
        }
    }
}

void Logger::Log(std::string&& log)
{
    if (LogContainer)
        LogContainer->push_back(log);
}

void Log(std::string&& log, bool log_to_stdout)
{
    GlobalLog->Log(move(log));
    if (log_to_stdout) std::cout << log << std::endl;
}

void FlushLog()
{
    GlobalLog->FlushLog();
}

void InitLogger()
{
    GlobalLog = new Logger;
}