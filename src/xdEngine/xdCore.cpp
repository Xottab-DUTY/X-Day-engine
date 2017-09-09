#include <ctime>
#include <sstream>
#include <filesystem>
namespace filesystem = std::experimental::filesystem;

#include <fmt/format.h>
#include <GLFW/glfw3.h>

#include "Common/Platform.hpp"
#include "Debug/Log.hpp"
#include "xdCore.hpp"
#include "XMLResource.hpp"

using namespace XDay;

XDAY_API xdCore Core;

static void error_callback(int error, const char* description)
{
    Error("GLFW Error: \nCode: {} \nMeans: {}", error, description);
}

bool xdCore::isGlobalDebug()
{
#ifdef DEBUG
    return true;
#endif
    return FindParam(eParamDebug);
}

xdCore::xdCore()
{
    EngineName = "X-Day Engine";
    EngineVersion = "1.0";
    CalculateBuildId();
}

void xdCore::InitializeArguments(int argc, char* argv[])
{
    for (int i = 0; i < argc; ++i)
        Params.push_back(argv[i]);

    for (auto&& str : Params)
        ParamsString += str + " ";

    ParamsString.pop_back(); // remove the last " "
}

void xdCore::Initialize(std::string&& _appname)
{
    Info("{} {} (build {})", EngineName, EngineVersion, buildId);
    DebugMsg("Core: Initializing");
    AppVersion = "1.0";
    FindParam(eParamName) ? AppName = ReturnParam(eParamName) : AppName = _appname;
    FindParam(eParamGame) ? GameModule = ReturnParam(eParamGame) : GameModule = "xdGame";
    AppPath = filesystem::absolute(Params.front());
    WorkPath = filesystem::current_path();
    BinPath = WorkPath.string() + "/bin/";
    FindParam(eParamDataPath) ? DataPath = ReturnParam(eParamDataPath) : DataPath = WorkPath.string() + "/appdata/";
    FindParam(eParamResPath) ? ResourcesPath = ReturnParam(eParamResPath) : ResourcesPath = WorkPath.string() + "/resources/";

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

    buildString = fmt::format("{} build {}, {}, {}", GetModuleName("xdCore"), buildId, buildDate, buildTime);
    GLFWVersionString = fmt::format("GLFW {}", glfwGetVersionString());
    glfwSetErrorCallback(error_callback);
    DebugMsg("Core: Initialized");
    GlobalLog.onCoreInitialized();
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
bool xdCore::FindParam(eCoreParams param) const
{
    if (ParamsString.find(RecognizeParam(param)) != std::string::npos)
        return true;
    return false;
}

// Finds command line parameter and returns it's value.
// If parameter isn't found it returns empty string.
// Do not use ReturnParam() if FindParam() returns false
// else you will get an unexpected behavior
std::string xdCore::ReturnParam(eCoreParams param) const
{
    bool found = false;
    auto p = RecognizeParam(param);
    for (auto i : Params)
    {
        if (found && i.find(RecognizeParam(eParamPrefix)) != std::string::npos)
        {
            Error("xdCore::ReturnParam(): wrong construction \"{0} {1}\" used instead of \"{0} *value* {1}\"", p, i);
            break;
        }
        if (found)
            return i;
        if (i.find(p) == std::string::npos)
            continue;
        found = true;
    }

    Error("xdCore::ReturnParam(): returning empty string for param {}", p);
    return "";
}

std::string xdCore::RecognizeParam(eCoreParams param) const
{
    switch (param)
    {
    case eParamPrefix:
        return "--p_";
    case eParamDebug:
        return "--p_debug";
    case eParamNoLog:
        return "--p_nolog";
    case eParamNoLogFlush:
        return "--p_nologflush";
    case eParamResPath:
        return "--p_respath";
    case eParamDataPath:
        return "--p_datapath";
    case eParamMainConfig:
        return "--p_mainconfig";
    case eParamName:
        return "--p_name";
    case eParamGame:
        return "--p_game";
    case eParamShaderForceRecompilation:
        return "--p_shrec";
    case eParamShaderPreprocess:
        return "--p_shpre";
    case eParamTexture:
        return "--p_texture";
    case eParamModel:
        return "--p_model";
    default:
        Log("xdCore::RecognizeParam():: How this happend? Passing empty string");
        return "";
    }
}

void xdCore::CreateDirIfNotExist(const filesystem::path& p) const
{
    if (!filesystem::exists(p)) filesystem::create_directory(p);
}

// Returns given module name with configuration and architecture
const std::string xdCore::GetModuleName(std::string&& xdModule)
{
#ifdef DEBUG
#ifdef XD_X64
    return xdModule + "_Dx64";
#else
    return xdModule + "_Dx86";
#endif
#elif NDEBUG
#ifdef XD_X64
    return xdModule + "_Rx64";
#else
    return xdModule + "_Rx86";
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
