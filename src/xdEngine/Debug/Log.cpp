#include "Log.hpp"

#include <fstream>
#include <iostream>

#include "xdCore.hpp"

XDAY_API CLog* Logger = nullptr;

CLog::CLog()
{
    LogContainer = new std::vector<std::string>();
    LogContainer->reserve(1000);
}



CLog::CLog(std::string _logfile)
{
    LogContainer = new std::vector<std::string>();
    LogContainer->reserve(1000);
    LogFile = _logfile;
}

void CLog::InitLog()
{
    if (LogContainer) CloseLog();
    if (Core.FindParam("--p_nolog")) nolog = true;
    if (Core.FindParam("--p_nologflush")) nologflush = true;
    if (!nolog)
    {
        Core.FindParam("--p_mainlog") ? LogFile = Core.ReturnParam("--p_mainlog") : LogFile = Core.LogsPath.string() + "main.log";
        LogContainer = new std::vector<std::string>();
        LogContainer->reserve(1000);
    }
}

void CLog::CloseLog()
{
    if (LogContainer)
    {
        FlushLog();
        LogContainer->clear();
        delete LogContainer;
    }
}

void CLog::FlushLog()
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

void CLog::Log(std::string&& log)
{
    if (LogContainer)
        LogContainer->push_back(log);
}

void Log(std::string&& log, bool log_to_stdout)
{
    Logger->Log(move(log));
    if (log_to_stdout) std::cout << log << std::endl;
}

void InitLogger()
{
    Logger = new CLog;
}