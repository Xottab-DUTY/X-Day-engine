#pragma once

#include <string>
#include <filesystem>
namespace filesystem = std::experimental::filesystem;

namespace XDay
{
namespace CommandLine
{
class Key;
}

enum class CoreParams
{
    Prefix,
    Debug,
    NoLog,
    NoLogFlush,
    ResPath,
    DataPath,
    MainConfig,
    Name,
    Game,
    DontHideSystemConsole,

    ShaderForceRecompilation,
    ShaderPreprocess,

    Texture,
    Model
};

class XDCORE_API xdCore
{
    double buildId = 0;
    const std::string buildDate = __DATE__;
    const std::string buildTime = __TIME__;
    std::string buildString;
    std::string GLFWVersionString;

public:
    std::string EngineName;
    std::string EngineVersion;

    std::string AppName;
    std::string AppVersion;
    filesystem::path AppPath;
    filesystem::path WorkPath;
    filesystem::path BinPath;

    filesystem::path DataPath;
    filesystem::path BinaryShadersPath;
    filesystem::path LogsPath;
    filesystem::path SavesPath;

    filesystem::path ResourcesPath;
    filesystem::path ArchivesPath;
    filesystem::path ConfigsPath;
    filesystem::path ModelsPath;
    filesystem::path ShadersPath;
    filesystem::path SoundsPath;
    filesystem::path TexturesPath;

    std::string UserName = "X-Day User";
    std::string CompName = "X-Day Computer";
    std::vector<std::string> Params;
    std::string ParamsString;

    bool isGlobalDebug() const;

public:
    xdCore();
    void InitializeArguments(int argc, char* argv[]);
    void Initialize(std::string&& ApplicationName = "X-Day Engine");
    void Destroy();

    auto GetBuildId() const { return buildId; }
    auto GetBuildDate() const { return buildDate; }
    auto GetBuildTime() const { return buildTime; }
    auto GetBuildString() const { return buildString; }
    auto GetGLFWVersionString() const { return GLFWVersionString; }
    auto GetParams() const { return Params; }
    auto GetParamsString() const { return ParamsString; }

    static void GetParamsHelp();

    void CreateDirIfNotExist(const filesystem::path& p) const;

    bool FindParam(stringc param) const;
    string ReturnParam(stringc param) const;

private:
    void InitializeResources();
    void CalculateBuildId();
};
}

extern XDCORE_API XDay::xdCore Core;
