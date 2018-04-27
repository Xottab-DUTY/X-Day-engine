#include "pch.hpp"

#include <ctime>
#include <sstream>
#include <filesystem>
namespace filesystem = std::experimental::filesystem;

#include <fmt/format.h>
#include <GLFW/glfw3.h>

#include "Core.hpp"
#include "CommandLine/Keys.hpp"
#include "ModuleManager.hpp"
#include "XML/XMLResource.hpp"

using namespace XDay;

XDCORE_API xdCore Core;

static void error_callback(int error, const char* description)
{
    Log::Error("GLFW Error: \nCode: {} \nMeans: {}", error, description);
}

bool xdCore::isGlobalDebug() const
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

    AppPath = filesystem::absolute(params.front());
    WorkPath = filesystem::current_path();
    BinPath = WorkPath.string() + "/bin/";

    auto& key = CommandLine::KeyName;
    key.IsSet() ? appName = key.StringValue() : appName = _appname;

    key = CommandLine::KeyDataPath;
    key.IsSet() ? DataPath = key.StringValue() : DataPath = WorkPath.string() + "/appdata/";

    key = CommandLine::KeyResPath;
    key.IsSet() ? ResourcesPath = key.StringValue() : ResourcesPath = WorkPath.string() + "/resources/";

    BinaryShadersPath = DataPath.string() + "/binary_shaders/";
    LogsPath = DataPath.string() + "/logs/";
    SavesPath = DataPath.string() + "/saves/";
    
    InitializeResources();

    CreateDirIfNotExist(DataPath);
    CreateDirIfNotExist(BinaryShadersPath);
    CreateDirIfNotExist(LogsPath);
    CreateDirIfNotExist(SavesPath);

    CreateDirIfNotExist(ResourcesPath);
    CreateDirIfNotExist(ArchivesPath);
    CreateDirIfNotExist(ConfigsPath);
    CreateDirIfNotExist(ModelsPath);
    CreateDirIfNotExist(ShadersPath);
    CreateDirIfNotExist(SoundsPath);
    CreateDirIfNotExist(TexturesPath);

    buildString = fmt::format("{} build {}, {}, {}", ModuleManager::GetModuleName(EngineModules::Core, false), buildId, buildDate, buildTime);
    glfwVersionString = fmt::format("GLFW {}", glfwGetVersionString());
    glfwSetErrorCallback(error_callback);
    Log::Debug("Core: Initialized");
    Log::onCoreInitialized();
}

void xdCore::Destroy()
{
    CommandLine::Keys::Destroy();
}


void xdCore::InitializeResources()
{
    ArchivesPath = ResourcesPath.string() + "/archives/";
    ConfigsPath = ResourcesPath.string() + "/configs/";
    ModelsPath = ResourcesPath.string() + "/models/";
    ShadersPath = ResourcesPath.string() + "/shaders/";
    SoundsPath = ResourcesPath.string() + "/sounds/";
    TexturesPath = ResourcesPath.string() + "/textures/";

    xdXMLResource resource_initializer(ResourcesPath, "resources.xml");
    if (!resource_initializer.isErrored())
        resource_initializer.ParseResources();
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

void xdCore::CreateDirIfNotExist(const filesystem::path& p) const
{
    if (!filesystem::exists(p)) filesystem::create_directory(p);
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
        startDate_tm.tm_hour = 12; //random hour(don't remember exact hour)
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
