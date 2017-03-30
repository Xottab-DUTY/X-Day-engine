#pragma once
#include <string>
#include <filesystem>
namespace filesystem = std::experimental::filesystem::v1;

#include <fmt/format.h>

#include "xdEngine.hpp"

class XDENGINE_API xdConsoleCommand;

class XDENGINE_API xdConsole
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

extern XDENGINE_API xdConsole* Console;
