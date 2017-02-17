#pragma once
#include <string>
#include <filesystem>
namespace filesystem = std::experimental::filesystem::v1;

#include "xdEngine.hpp"

class XDENGINE_API xdCore
{
private:
    std::string buildDate = __DATE__;
    std::string buildTime = __TIME__;
    unsigned buildId;

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
    

    std::string UserName = u8"X-Day User";
    //std::string CompName = u8"X-Day Computer";
    std::string Params;

public:
    void Initialize(std::string&& ApplicationName, char& argv);
    void Destroy();
    const std::string GetBuildDate() const { return buildDate; }
    auto GetBuildId() const { return buildId; }
    //void LoadModule(std::string&& ModuleName, bool&& log);
    //void UnloadModule(std::string&& ModuleName, bool&& log);
    void CreateDirIfNotExist(filesystem::path&& p);
    bool FindParam(std::string&& Param);
    std::string ReturnParam(std::string&& Param);

private:
    void CalculateBuildId();
};

extern XDENGINE_API xdCore Core;