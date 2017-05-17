#include "xdEngine.hpp"
#include "xdCore.hpp"
#include "Console.hpp"
#include "ConsoleCommand.hpp"

XDAY_API xdConsole* Console = nullptr;
XDAY_API CC_Container* ConsoleCommands = nullptr;

void xdConsole::Initialize()
{
    Core.FindParam("--p_mainconfig") ? ConfigFile = Core.ReturnParam("--p_mainconfig") : ConfigFile = Core.DataPath.string() + "main.config";

    RegisterConsoleCommands();
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
