#include <string>
#include <fstream>
#include <iostream>

#include "xdEngine.hpp"
#include "Console.hpp"
#include "xdCore.hpp"

XDENGINE_API xdConsole* Console = nullptr;

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
            Console->Log("Flushing log..", true);
            std::ofstream f;
            f.open(LogFile);
            //for (auto itr = LogContainer->cbegin(); itr != LogContainer->cend(); ++itr) // TODO: fix for below issue, but body cycle broken then
            for (auto i = 0; i < LogContainer->size(); i++) // TODO: fixed size type(int) < dynamic size type(std::vector). Needs fix.
            {
                f << LogContainer->data()[i] << std::endl; // TODO: dynamic size type(std::vector)[fixed size type(int)]. Child from above issue. Needs fix.
            }
            f.close();
        }
    }
}
