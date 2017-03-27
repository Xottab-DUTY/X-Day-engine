#pragma once
//#include <map>
#include <string>
#include <filesystem>
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
    //std::map<std::string, xdConsoleCommand> CommandsContainer;

public:
    void Initialize();
    void InitLog();
    void CloseLog();
    void FlushLog();

    void Log(std::string&& log, bool cmd = true);
    //void AddCommand(xdConsoleCommand&& cmdName);
    //void RemoveCommand(xdConsoleCommand&& cmdName);

    void Show();
    void Hide();
};

extern XDENGINE_API xdConsole* Console;
