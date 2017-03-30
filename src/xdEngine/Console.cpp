#include <string>
#include <fstream>
#include <iostream>

#include "xdEngine.hpp"
#include "xdCore.hpp"
#include "Console.hpp"
#include "ConsoleCommand.hpp"

XDAY_API xdConsole* Console = nullptr;
XDAY_API xdCC_Container* ConsoleCommands = nullptr;

void xdConsole::Initialize()
{
    Core.FindParam("-mainconfig") ? ConfigFile = Core.ReturnParam("-mainconfig") : ConfigFile = Core.DataPath.string() + "main.config";
    Core.FindParam("-mainlog") ? LogFile = Core.ReturnParam("-mainlog") : LogFile = Core.DataPath.string() + "main.log";
    Console->InitLog();
}

void xdConsole::Show()
{
    if (isVisible)
        return;
    isVisible = true;
}

void xdConsole::Hide()
{
    if (!isVisible)
        return;
    isVisible = false;
}

void xdConsole::InitLog()
{
    if (Core.FindParam("-nolog")) nolog = true;
    if (Core.FindParam("-nologflush")) nologflush = true;
    if (!nolog)
    {
        LogContainer = new std::vector<std::string>();
        LogContainer->reserve(1000);
    }
}

void xdConsole::CloseLog()
{
    if (!nolog)
    {
        if (LogContainer)
        {
            FlushLog();
            LogContainer->clear();
            delete LogContainer;
        }
    }
}

void xdConsole::Log(std::string&& log, bool cmd)
{
    if (!nolog)
    {
        LogContainer->push_back(log);
        if (cmd) std::cout << log << std::endl;
    }
}

void xdConsole::FlushLog()
{
    if (!nolog)
    {
        if (!nologflush)
        {
            Console->Log("Flushing log..");
            std::ofstream f(LogFile);
            for (auto&& str : *LogContainer)
            {
                f << str << std::endl;
            }
            f.close();
        }
    }
}
