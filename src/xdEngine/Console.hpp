#pragma once
#ifndef Console_hpp__
#define Console_hpp__

#include <filesystem>
namespace filesystem = std::experimental::filesystem::v1;

#include <fmt/format.h>

#include "xdEngine_impexp.inl"

class XDAY_API xdConsole
{
public:
    bool isVisible = false;
    bool nolog = false;
    bool nologflush = false;
    filesystem::path ConfigFile;
    filesystem::path LogFile;
    std::vector<std::string>* LogContainer = nullptr;

public:
    void Initialize();
    void InitLog();
    void CloseLog();
    void FlushLog();

#define ConsoleMsg(log, ...) { Console->Log(fmt::format(log, __VA_ARGS__)); }
    void Log(std::string&& log, bool cmd = true);

    void Show();
    void Hide();
};

extern XDAY_API xdConsole* Console;

#endif // Console_hpp__
