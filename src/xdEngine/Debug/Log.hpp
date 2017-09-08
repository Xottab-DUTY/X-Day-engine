#pragma once
#ifndef Log_hpp__
#define Log_hpp__
#include <filesystem>
namespace filesystem = std::experimental::filesystem;

#include <fmt/format.h>

#include "xdEngine_impexp.inl"

namespace XDay
{
class XDAY_API Logger
{
    bool nolog = false;
    bool nologflush = false;
    filesystem::path LogFile = "X-Day.init.log";
    std::vector<std::string>* LogContainer = nullptr;

public:
    Logger();
    Logger(std::string&& _logfile);


    void InitLog(); // Used only in GlobalLog
    void CloseLog();
    void FlushLog();


    void Log(std::string&& log);
};
}

extern XDAY_API XDay::Logger* GlobalLog;

#define Msg(log, ...) { Log(fmt::format(log, __VA_ARGS__)); }
void XDAY_API Log(std::string&& log, bool log_to_stdout = true);

void XDAY_API FlushLog();

#endif // Log_hpp__