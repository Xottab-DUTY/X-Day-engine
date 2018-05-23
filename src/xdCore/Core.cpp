#include "pch.hpp"

#include <ctime>
#include <sstream>

#include <fmt/format.h>
#include <GLFW/glfw3.h>

#include "Core.hpp"
#include "CommandLine/Keys.hpp"
#include "Console/ConsoleCommands.hpp"
#include "Filesystem.hpp"
#include "Module.hpp"

using namespace XDay;

XDCORE_API xdCore Core;

static void error_callback(int error, const char* description)
{
    Log::Error("GLFW Error: \nCode: {} \nMeans: {}", error, description);
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
    engineName = "X-Day Engine";
    engineVersion = "1.0";
    CalculateBuildId();
}

void xdCore::InitializeArguments(int argc, char* argv[])
{
    for (int i = 0; i < argc; ++i)
        params.push_back(argv[i]);

    for (auto&& str : params)
        paramsString += str + " ";

    paramsString.pop_back(); // remove the last " "

    CommandLine::Keys::Initialize();
}

void xdCore::Initialize(stringc&& _appname)
{
    Log::Info("{} {} (build {})", engineName, engineVersion, buildId);
    Log::Debug("Core: Initializing");
    appVersion = "1.0";

    auto& key = CommandLine::KeyName;
    key.IsSet() ? appName = key.StringValue() : appName = _appname;

    buildString = fmt::format("{} {} {} (build {}, {}, {})", engineName, engineVersion, GetBuildConfiguration(), buildId, buildDate, buildTime);
    glfwVersionString = fmt::format("GLFW {}", glfwGetVersionString());
    glfwSetErrorCallback(error_callback);

    FS.Initialize();

    Log::Debug("Core: Initialized");
    Log::onCoreInitialized();
    Console::Commands::Initialize();
}

void xdCore::Destroy()
{
    CommandLine::Keys::Destroy();
    Console::Commands::Destroy();
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
string xdCore::ReturnParam(stringc param) const
{
    bool found = false;
    for (auto& i : params)
    {
        if (found && i.find(CommandLine::KeyPrefix) != string::npos)
        {
            Log::Error("xdCore::ReturnParam(): wrong construction [{0} {1}] used instead of [{0} *value* {1}]", param, i);
            break;
        }
        if (found)
            return i;
        if (i.find(CommandLine::KeyPrefix + param) == string::npos)
            continue;
        found = true;
    }

    Log::Error("xdCore::ReturnParam(): returning empty string for param [{}]", param);
    return "";
}

constexpr pcstr xdCore::GetBuildConfiguration()
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

    buildId = -difftime(std::mktime(&startDate_tm), std::mktime(&buildDate_tm)) / 86400;
}
