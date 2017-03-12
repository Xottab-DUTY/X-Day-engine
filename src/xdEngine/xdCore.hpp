#pragma once
#include <string>

#include <sstream>
#include <filesystem>
namespace filesystem = std::experimental::filesystem::v1;

#include "xdEngine.hpp"

class XDENGINE_API xdCore
{
private:
    double buildId = 0;
    const std::string buildDate = __DATE__;
    const std::string buildTime = __TIME__;
    std::string buildString;

public:
    std::string AppName;
    filesystem::path AppPath;
    filesystem::path WorkPath;
    filesystem::path binPath;
    filesystem::path DataPath;
    filesystem::path ResPath;
    filesystem::path configsPath;
    filesystem::path texturesPath;
    filesystem::path modelsPath;
    filesystem::path soundsPath;
    filesystem::path archivesPath;

    std::string GameModule;
    
    std::string UserName = "X-Day User";
    std::string CompName = "X-Day Computer";
    std::string Params;

public:
    void Initialize(std::string&& ApplicationName, const char& argv);
    void Destroy();
    auto GetBuildId() const { return buildId; }
    auto GetBuildDate() const { return buildDate; }
    auto GetBuildTime() const { return buildTime; }
    auto GetBuildString() const { return buildString; }
    void CreateDirIfNotExist(filesystem::path&& p);
    bool FindParam(std::string&& Param);
    std::string ReturnParam(std::string&& Param);

private:
    void CalculateBuildId();
};

extern XDENGINE_API xdCore Core;
