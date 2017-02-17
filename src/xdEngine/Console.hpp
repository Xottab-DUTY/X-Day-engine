#pragma once
#include <filesystem>
#include <thread>
namespace filesystem = std::experimental::filesystem::v1;

#include "xdEngine.hpp"
#include "ConsoleCommand.hpp"

class XDENGINE_API xdConsole
{
public:
    bool isVisible = false;
    bool nolog = false;
    bool nologflush = false;
    filesystem::path ConfigFile;
    filesystem::path LogFile;
    std::vector<std::string>* LogContainer = nullptr;
    std::vector<std::string>* CmdContainer = nullptr;
public:
    void Initialize();
    void InitLog();
    void CloseLog();
    void FlushLog();

    void Log(std::string&& log, bool cmd);

    //void AddCommand(xdConsoleCommand cmd);
    //void RemoveCommand(xdConsoleCommand cmd);

    void Show();
    void Hide();
};

extern XDENGINE_API xdConsole* Console;