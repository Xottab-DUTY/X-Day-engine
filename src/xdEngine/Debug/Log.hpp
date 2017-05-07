#pragma once
#ifndef Log_hpp__
#define Log_hpp__
#include <filesystem>
namespace filesystem = std::experimental::filesystem::v1;

#include "xdEngine_impexp.inl"

class XDAY_API CLog
{
    bool nolog = false;
    bool nologflush = false;
    filesystem::path LogFile = "init.log";
    std::vector<std::string>* LogContainer = nullptr;

public:
    CLog();

    CLog(std::string _logfile);

    void InitLog();
    void CloseLog();
    void FlushLog();


    void Log(std::string&& log);
};

extern XDAY_API CLog* Logger;

void InitLogger();

#define Msg(log, ...) { Log(fmt::format(log, __VA_ARGS__)); }
void Log(std::string&& log, bool log_to_stdout = true);

#endif // Log_hpp__