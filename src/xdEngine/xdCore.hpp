#pragma once
#include <string>
#include <sstream>
#include <filesystem>
namespace filesystem = std::experimental::filesystem::v1;

#include <GLFW/glfw3.h>

#include "xdEngine.hpp"

using importedFunction = void(*)();

class XDENGINE_API xdCore
{
private:
    double buildId = 0;
    const std::string buildDate = __DATE__;
    const std::string buildTime = __TIME__;
    std::string buildString;
    std::string GLFWVersionString;

public:
    std::string AppName;
    filesystem::path AppPath;
    filesystem::path WorkPath;
    filesystem::path BinPath;
    filesystem::path DataPath;
    filesystem::path ResPath;
    filesystem::path ArchivesPath;
    filesystem::path ConfigsPath;
    filesystem::path ModelsPath;
    filesystem::path SoundsPath;
    filesystem::path TexturesPath;

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
    auto GetGLFWVersionString() const { return GLFWVersionString; }
    void CreateDirIfNotExist(const filesystem::path& p);
    bool FindParam(std::string&& Param);
    std::string ReturnParam(std::string&& Param);

private:
    void CalculateBuildId();
};

extern XDENGINE_API xdCore Core;
