#include "pch.hpp"

#include "xdCore/CommandLine.hpp"
#include "xdCore/Filesystem.hpp"
#include "Console.hpp"
#include "ConsoleCommands.hpp"

namespace XDay::CommandLine
{
Key KeyMainConfig("mainconfig", "Specifies path and name of main config file (path/name.extension), default is \"*DataPath*/main.config\"", KeyType::String);
}

namespace XDay::Console
{
XDENGINE_API CConsole Console;
void CConsole::Initialize()
{
    CommandLine::KeyMainConfig.IsSet()
        ? ConfigFile = CommandLine::KeyMainConfig.StringValue()
        : ConfigFile = FS.DataPath.string() + "main.config";

    RegisterEngineCommands();
}

void CConsole::ExecuteConfig(const filesystem::path& path)
{
    ConfigLoad.Execute(path.string());
}
} // namespace XDay::::Console
