#include <ctime>
#include <filesystem>
namespace filesystem = std::experimental::filesystem::v1;

#include <fmt/format.h>
#include <GLFW/glfw3.h>

#include "xdCore.hpp"
#include "Console.hpp"
#include "Platform/Platform.hpp"

XDENGINE_API xdCore Core;

static void error_callback(int error, const char* description)
{
    Console->Log(fmt::format("GLFW Error: {}", description));
}

void xdCore::Initialize(std::string&& _AppName, const char& argv)
{
    FindParam("-name") ? AppName = ReturnParam("-name") : AppName = _AppName;
    FindParam("-game") ? GameModule = ReturnParam("-game") : GameModule = "xdGame";
    Params = &argv;
    AppPath = filesystem::absolute(&Params[0]);
    WorkPath = filesystem::current_path();
    BinPath = WorkPath.string() + "/bin/";
    FindParam("-datapath") ? DataPath = ReturnParam("-datapath") : DataPath = WorkPath.string() + "/appdata/";
    FindParam("-respath") ? ResPath = ReturnParam("-respath") : ResPath = WorkPath.string() + "/res/";

    ArchivesPath = ResPath.string() + "/archives/";
    ConfigsPath = ResPath.string() + "/configs/";
    ModelsPath = ResPath.string() + "/models/";
    SoundsPath = ResPath.string() + "/sounds/";
    TexturesPath = ResPath.string() + "/textures/";

    CreateDirIfNotExist(DataPath);
    CreateDirIfNotExist(ResPath);
    CreateDirIfNotExist(ArchivesPath);
    CreateDirIfNotExist(ConfigsPath);
    CreateDirIfNotExist(ModelsPath);
    CreateDirIfNotExist(SoundsPath);
    CreateDirIfNotExist(TexturesPath);

    CalculateBuildId();

    buildString = fmt::format("xdCore build {}, {}, {}", buildId, buildDate, buildTime);
    GLFWVersionString = fmt::format("GLFW {}", glfwGetVersionString());
    glfwSetErrorCallback(error_callback);
}

// Finds command line parameters and returns true if param exists
bool xdCore::FindParam(std::string&& Param)
{
    if (Params.find(Param) != std::string::npos)
        return true;
    return false;
}

// Finds command line parameter and returns it's value.
// Keep in mind that command line parameter:
// -name value will return "alue"
// -name "value" will return correct "value"
// Do not use ReturnParam() if FindParam() returns false
// else you will get an unexpected behavior
std::string xdCore::ReturnParam(std::string&& Param)
{
    Param = Params.substr(Params.find(Param) + (Param.length() + 2), Params.find(Param + " %[^ ]"));
    Param.pop_back(); // Costyl (TM)
    return Param;
}

void xdCore::CreateDirIfNotExist(const filesystem::path& p)
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
    Console->Log("Something went wrong when calling GetModuleName()");
    return xdModule;
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
