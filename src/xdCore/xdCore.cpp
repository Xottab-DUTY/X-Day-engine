#include "pch.hpp"

#include <ctime>
#include <sstream>
#include <filesystem>
namespace filesystem = std::experimental::filesystem;

#include <fmt/format.h>
#include <GLFW/glfw3.h>

#include "Common/Platform.hpp"
#include "Log.hpp"
#include "xdCore.hpp"
#include "XML/XMLResource.hpp"

using namespace XDay;

XDCORE_API xdCore Core;

static void error_callback(int error, const char* description)
{
    Error("GLFW Error: \nCode: {} \nMeans: {}", error, description);
}

bool xdCore::isGlobalDebug() const
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

    buildString = fmt::format("{} build {}, {}, {}", GetModuleName(eCoreModule, false), buildId, buildDate, buildTime);
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
    const auto p = RecognizeParam(param);
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

std::string xdCore::RecognizeParam(eCoreParams param)
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
    case eParamDontHideSystemConsole:
        return "--p_syscmd";
    case eParamShaderForceRecompilation:
        return "--p_shrec";
    case eParamShaderPreprocess:
        return "--p_shpre";
    case eParamTexture:
        return "--p_texture";
    case eParamModel:
        return "--p_model";
    default:
        throw "Create the case for the param here!";
    }
}

void xdCore::GetParamsHelp()
{
    Info("\nAvailable parameters:\n"\
         "--p_name - Specifies AppName, default is \"X-Day Engine\" \n"\
         "--p_game - Specifies game module to be attached, default is \"xdGame\";\n"\
         "--p_datapath - Specifies path of application data folder, default is \"*WorkingDirectory*/appdata\"\n"\
         "--p_respath - Specifies path of resources folder, default is \"*WorkingDirectory*/resources\"\n"\
         "--p_mainconfig - Specifies path and name of main config file (path/name.extension), default is \"*DataPath*/main.config\" \n"\
         "--p_nolog - Completely disables engine log. May increase performance\n"\
         "--p_nologflush - Disables log flushing. Useless if -nolog defined\n"\
         "--p_debug - Enables debug mode\n"
         "--p_syscmd - Disables system console hiding\n"
         "--p_shrec - Compile shaders even if they already compiled\n"\
         "--p_shpre - Outputs preprocessed shaders to the shader sources dir. Works only in debug mode\n"\
         "--p_texture - Specifies path to texture file to load, default is \"texture.dds\"\n"\
         "--p_model - Specifies path to model file to model, default is \"model.dds\"\n");

    Info("\nДоступные параметры:\n"\
         "--p_name - Задаёт AppName, по умолчанию: \"X-Day Engine\" \n"\
         "--p_game - Задаёт игровую библиотеку для подключения, по умолчанию: \"xdGame\";\n"
         "--p_datapath - Задаёт путь до папки с настройками, по умолчанию: \"*WorkingDirectory*/appdata\"\n"\
         "--p_respath - Задаёт путь до папки с ресурсами, по умолчанию: \"*WorkingDirectory*/resources\"\n"\
         "--p_mainconfig - Задаёт путь и имя главного файла настроек (путь/имя.расширение), по умолчанию: \"*DataPath*/main.config\" \n"\
         "--p_nolog - Полностью выключает лог движка. Может повысить производительность\n"\
         "--p_nologflush - Выключает сброс лога в файл. Не имеет смысла если задан -nolog\n"\
         "--p_debug - Включает режим отладки\n"
         "--p_syscmd - Отключает скрытие системной консоли\n"
         "--p_shrec - Сборка шейдеров даже если они уже собраны\n"\
         "--p_shpre - Сохраняет обработанные шейдеры в папку исходников шейдеров. Работает только в режиме отладки\n"\
         "--p_texture - Задаёт путь до текстуры для загрузки, по умолчанию: \"texture.dds\"\n"\
         "--p_model - Задаёт путь до модели для загрузки, по умолчанию: \"model.dds\"\n");
}

void xdCore::CreateDirIfNotExist(const filesystem::path& p) const
{
    if (!filesystem::exists(p)) filesystem::create_directory(p);
}

// Returns module name with configuration, architecture and extension
std::string xdCore::GetModuleName(eEngineModules xdModule, bool needExt)
{
    switch (xdModule)
    {
    case eEngineModule:
        return GetModuleName("X-Day", needExt, true);
    case eCoreModule:
        return GetModuleName("X-Day.Core", needExt);
    case eAPIModule:
        return GetModuleName("X-Day.API", needExt);
    case eRendererModule:
        return GetModuleName("X-Day.Renderer", needExt);
    default:
        throw "Create the case for the module here!";
    }
}

// Returns given module name with configuration and architecture and extension
std::string xdCore::GetModuleName(std::string&& xdModule, bool needExt, bool isExecutable)
{
#ifdef DEBUG
#ifdef XD_X64
    if (needExt)
        return GetModuleExtension(xdModule + ".Dx64", isExecutable);
    return xdModule + ".Dx64";
#else
    if (needExt)
        return GetModuleExtension(xdModule + ".Dx86", isExecutable);
    return xdModule + ".Dx86";
#endif
#elif NDEBUG
#ifdef XD_X64
    if (needExt)
        return GetModuleExtension(xdModule + ".Rx64", isExecutable);
    return xdModule + ".Rx64";
#else
    if (needExt)
        return GetModuleExtension(xdModule + ".Rx86", isExecutable);
    return xdModule + ".Rx86";
#endif
#endif
}

// Returns given module name with extension
std::string xdCore::GetModuleExtension(std::string&& xdModule, bool isExecutable)
{
#ifdef WINDOWS
    if (isExecutable)
        return xdModule + ".exe";
    return xdModule + ".dll";
#elif defined(LINUX)
    if (isExecutable)
        return xdModule;
    return xdModule + ".so";
#endif
    return xdModule;
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
