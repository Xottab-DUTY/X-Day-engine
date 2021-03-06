#pragma once

namespace XDay
{
class XDCORE_API xdCore
{
    double buildId = 0;
    cpcstr buildDate = __DATE__;
    cpcstr buildTime = __TIME__;
    pcstr buildString;
    pcstr glfwVersionString;

    pcstr engineName;
    pcstr engineVersion;

    vector<string> params;
    string paramsString;

    pcstr appName;
    pcstr appVersion;

    pcstr userName;
    pcstr compName;

public:
    xdCore();
    void InitializeArguments(const int argc, char* argv[]);
    void Initialize();
    void Destroy();

    static bool isGlobalDebug();
    bool FindParam(stringc param) const;
    cpcstr ReturnParam(stringc param) const;

    auto GetBuildId() const { return buildId; }
    auto GetBuildDate() const { return buildDate; }
    auto GetBuildTime() const { return buildTime; }
    auto GetBuildString() const { return buildString; }
    auto GetGLFWVersionString() const { return glfwVersionString; }
    constexpr static cpcstr GetBuildConfiguration();

    auto GetEngineName() const { return engineName; }
    auto GetEngineVersion() const { return engineVersion; }

    auto GetParams() const { return params; }
    auto GetParamsString() const { return paramsString; }

    auto GetAppName() const { return appName; }
    auto GetAppVersion() const { return appVersion; }

    auto GetUsername() const { return userName; }
    auto GetCompname() const { return compName; }

private:
    void CalculateBuildId();
};
} // namespace XDay

extern XDCORE_API XDay::xdCore Core;
