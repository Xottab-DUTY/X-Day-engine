#include "pch.hpp"

#include <ctime>
#include <sstream>

#include <fmt/format.h>
#include <GLFW/glfw3.h>

#include "Core.hpp"
#include "Debug.hpp"
#include "CommandLine.hpp"
#include "Console/ConsoleCommands.hpp"
#include "Filesystem.hpp"

static void error_callback(int error, const char* description)
{
    XDay::Log::Error("GLFW Error: \nCode: {} \nMeans: {}", error, description);
}

XDCORE_API XDay::xdCore Core;

namespace XDay
{
namespace CommandLine
{
Key KeyDebug("debug", "Enables debug mode", KeyType::Boolean);
}

bool xdCore::isGlobalDebug()
{
#ifdef DEBUG
    return true;
#endif
    return CommandLine::KeyDebug.IsSet();
}

xdCore::xdCore()
{
    CalculateBuildId();
    engineName = "X-Day Engine";
    engineVersion = "1.0";
    buildString = fmt::format("{} {} {} (build {}, {}, {})",
                         engineName, engineVersion,
                         GetBuildConfiguration(), buildId,
                         buildDate, buildTime).c_str();

    glfwVersionString = fmt::format("GLFW {}", glfwGetVersionString()).c_str();

    appName = "X-Day Application";
    appVersion = "1.0";
    userName = "X-Day User";
    compName = "X-Day Computer";
}

void xdCore::InitializeArguments(const int argc, char* argv[])
{
    for (int i = 0; i < argc; ++i)
        params.push_back(argv[i]);

    for (const auto& str : params)
        paramsString += str + " ";

    paramsString.pop_back(); // remove the last " "

    CommandLine::Keys::Initialize();
}

void xdCore::Initialize()
{
    Filesystem::BasicInit();
    FS.Initialize();
    Log::Initialize();
    Log::Info(Core.GetBuildString());
    Log::Info(Core.GetGLFWVersionString());
    Log::Info("{}: {}", "Core.Params", Core.GetParamsString());
    Log::Info("Девиз: Чем стрелы коленом ловить, гораздо интереснее отстреливать свои ноги. Продолжим.");
    Log::Info("Slogan: It's more interesting to shoot your feet, than catch arrows by your knee. Let's continue.");

    glfwSetErrorCallback(error_callback);

    Console::Commands::Initialize();
}

void xdCore::Destroy()
{
    CommandLine::Keys::Destroy();
    Console::Commands::Destroy();
    Filesystem::Destroy();
}

// Finds command line parameters and returns true if param exists
bool xdCore::FindParam(stringc param) const
{
    if (paramsString.find(CommandLine::KeyPrefix + param) != string::npos)
        return true;
    return false;
}

// Finds command line parameter and returns it's value.
// If parameter isn't found it returns empty string.
// Do not use ReturnParam() if FindParam() returns false
// else you will get an unexpected behavior
cpcstr xdCore::ReturnParam(stringc param) const
{
    bool found = false;
    for (const auto& i : params)
    {
        if (found && i.find(CommandLine::KeyPrefix) != string::npos)
        {
            Log::Error("{}:: wrong construction [{1} {2}] used instead of [{1} *value* {2}]", __FUNCTION__, param, i);
            break;
        }
        if (found)
            return i.c_str();
        if (i.find(CommandLine::KeyPrefix + param) == string::npos)
            continue;
        found = true;
    }

    Log::Error("{}:: returning empty string for param [{}]", __FUNCTION__, param);
    return "";
}

constexpr cpcstr xdCore::GetBuildConfiguration()
{
#ifdef DEBUG
#ifdef XR_X64
    return "Dx64";
#else
    return "Dx86";
#endif
#else
#ifdef XR_X64
    return "Rx64";
#else
    return "Rx86";
#endif
#endif
}

void xdCore::CalculateBuildId()
{
    // All started in ~01.01.2017
    std::tm startDate_tm;
    {
        // Start date and time
        startDate_tm.tm_mday = 1;
        startDate_tm.tm_mon = 0;
        startDate_tm.tm_year = 2017 - 1900;
        startDate_tm.tm_hour = 12; // I don't remember the exact hour of creation, this is just a random number
        startDate_tm.tm_min = 0;
        startDate_tm.tm_sec = 0;
    }

    std::tm buildDate_tm;
    {
        // Build date
        std::string stringMonth;
        std::istringstream buffer(buildDate);
        buffer >> stringMonth >> buildDate_tm.tm_mday >> buildDate_tm.tm_year;
        buildDate_tm.tm_year -= 1900;

        const char* monthId[12] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };
        for (int i = 0; i < 12; i++)
        {
            if (stringMonth.compare(monthId[i])) continue;
            buildDate_tm.tm_mon = i;
            break;
        }
    }

    {
        // Build time
        std::string timeBuffer(buildTime);
        replace(timeBuffer.begin(), timeBuffer.end(), ':', ' '); // Costyl (TM)
        std::istringstream buffer2(timeBuffer);
        buffer2 >> buildDate_tm.tm_hour >> buildDate_tm.tm_min >> buildDate_tm.tm_sec;
    }

    buildId = -std::difftime(std::mktime(&startDate_tm), std::mktime(&buildDate_tm)) / 86400;
}
} // namespace XDay
