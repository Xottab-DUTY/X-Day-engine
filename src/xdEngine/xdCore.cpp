#include <ctime>
#include <sstream>
#include <filesystem>
namespace filesystem = std::experimental::filesystem::v1;

#include <fmt/format.h>
#include <GLFW/glfw3.h>

#include "Common/Platform.hpp"
#include "Debug/Log.hpp"
#include "xdCore.hpp"
#include "XMLResource.hpp"

XDAY_API xdCore Core;

static void error_callback(int error, const char* description)
{
    Msg("GLFW Error: Code: {}, Means: {}", error, description);
}

xdCore::xdCore()
{
    EngineName = "X-Day Engine";
    EngineVersion = "1.0";
}

void xdCore::Initialize(std::string&& _AppName, const char& argv)
{
    Params = &argv;
    FindParam("-name") ? AppName = ReturnParam("-name") : AppName = _AppName;
    FindParam("-game") ? GameModule = ReturnParam("-game") : GameModule = "xdGame";
    AppPath = filesystem::absolute(&Params[0]);
    WorkPath = filesystem::current_path();
    BinPath = WorkPath.string() + "/bin/";
    FindParam("-datapath") ? DataPath = ReturnParam("-datapath") : DataPath = WorkPath.string() + "/appdata/";
    FindParam("-respath") ? ResourcesPath = ReturnParam("-respath") : ResourcesPath = WorkPath.string() + "/resources/";

    LogsPath = DataPath.string() + "/logs/";
    SavesPath = DataPath.string() + "/saves/";
    
    InitializeResources();

    CreateDirIfNotExist(DataPath);
    CreateDirIfNotExist(LogsPath);
    CreateDirIfNotExist(SavesPath);

    CreateDirIfNotExist(ResourcesPath);
    CreateDirIfNotExist(ArchivesPath);
    CreateDirIfNotExist(ConfigsPath);
    CreateDirIfNotExist(ModelsPath);
    CreateDirIfNotExist(SoundsPath);
    CreateDirIfNotExist(TexturesPath);

    CalculateBuildId();

    buildString = fmt::format("{} build {}, {}, {}", GetModuleName("xdCore"), buildId, buildDate, buildTime);
    GLFWVersionString = fmt::format("GLFW {}", glfwGetVersionString());
    glfwSetErrorCallback(error_callback);
    Logger->InitLog();
}

void xdCore::InitializeResources()
{
    ArchivesPath = ResourcesPath.string() + "/archives/";
    ConfigsPath = ResourcesPath.string() + "/configs/";
    ModelsPath = ResourcesPath.string() + "/models/";
    SoundsPath = ResourcesPath.string() + "/sounds/";
    TexturesPath = ResourcesPath.string() + "/textures/";

    xdXMLResource resource_initializer(Core.ResourcesPath, "resources.xml");
    if (!resource_initializer.isErrored())
        resource_initializer.ParseResources();
}

// Finds command line parameters and returns true if param exists
bool xdCore::FindParam(std::string&& Param) const
{
    if (Params.find(Param) != std::string::npos)
        return true;
    return false;
}

// Finds command line parameter and returns it's value.
// Do not use ReturnParam() if FindParam() returns false
// else you will get an unexpected behavior
std::string xdCore::ReturnParam(std::string&& Param) const
{
    return Param;
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
    { // Start date and time
        startDate_tm.tm_mday = 1;
        startDate_tm.tm_mon = 0;
        startDate_tm.tm_year = 2017 - 1900;
        startDate_tm.tm_hour = 12; //random hour(don't remember exact hour)
        startDate_tm.tm_min = 0;
        startDate_tm.tm_sec = 0;
    }

    std::tm buildDate_tm;
    { // Build date
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
    { // Build time
        std::string timeBuffer(buildTime);
        std::replace(timeBuffer.begin(), timeBuffer.end(), ':', ' '); // Costyl (TM)
        std::istringstream buffer2(timeBuffer);
        buffer2 >> buildDate_tm.tm_hour >> buildDate_tm.tm_min >> buildDate_tm.tm_sec;
    }

    buildId = -difftime(std::mktime(&startDate_tm), std::mktime(&buildDate_tm)) / 86400;
}
