#pragma once
#ifndef xdCore_hpp__
#define xdCore_hpp__
#include <string>
#include <filesystem>
namespace filesystem = std::experimental::filesystem::v1;

#include "xdEngine_impexp.inl"

using FunctionPointer = void(*)();

namespace XDay
{
class XDAY_API xdCore
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

    std::string GameModule;

    std::string UserName = "X-Day User";
    std::string CompName = "X-Day Computer";
    std::vector<std::string> Params;
    std::string ParamsString;

    bool isGlobalDebug();

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

    void CreateDirIfNotExist(const filesystem::path& p) const;
    bool FindParam(std::string&& Param) const;
    std::string ReturnParam(std::string&& Param) const;

    static const std::string GetModuleName(std::string&& xdModule);

private:
    void InitializeResources();
    void CalculateBuildId();
};
}

extern XDAY_API XDay::xdCore Core;

#endif // xdCore_hpp__
