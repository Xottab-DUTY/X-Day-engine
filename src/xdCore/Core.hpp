#pragma once

// XXX: move the filesystem stuff out of Core
#include <filesystem>
namespace filesystem = std::experimental::filesystem;

namespace XDay
{
namespace CommandLine
{
class Key;
}

class XDCORE_API xdCore
{
    double buildId = 0;
    const string buildDate = __DATE__;
    const string buildTime = __TIME__;
    string buildString;
    string glfwVersionString;

    string engineName;
    string engineVersion;

    vector<string> params;
    string paramsString;

    string appName;
    string appVersion;

    string userName = "X-Day User";
    string compName = "X-Day Computer";

public:
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

    bool isGlobalDebug() const;

public:
    xdCore();
    void InitializeArguments(int argc, char* argv[]);
    void Initialize(stringc&& _appname = "X-Day Engine");
    void Destroy();

    auto GetBuildId() const { return buildId; }
    auto GetBuildDate() const { return buildDate; }
    auto GetBuildTime() const { return buildTime; }
    auto GetBuildString() const { return buildString; }
    auto GetGLFWVersionString() const { return glfwVersionString; }

    auto GetEngineName() const { return engineName; }
    auto GetEngineVersion() const { return engineVersion; }

    auto GetParams() const { return params; }
    auto GetParamsString() const { return paramsString; }

    auto GetAppName() const { return appName; }
    auto GetAppVersion() const { return appVersion; }

    auto GetUsername() const { return userName; }
    auto GetCompname() const { return compName; }

    void CreateDirIfNotExist(const filesystem::path& p) const;

    bool FindParam(stringc param) const;
    string ReturnParam(stringc param) const;

private:
    void InitializeResources();
    void CalculateBuildId();
};
}

extern XDCORE_API XDay::xdCore Core;
