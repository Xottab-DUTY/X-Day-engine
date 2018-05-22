#pragma once

namespace XDay
{
class XDCORE_API xdCore
{
    double buildId = 0;
    cpcstr buildDate = __DATE__;
    cpcstr buildTime = __TIME__;
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
    xdCore();
    void InitializeArguments(int argc, char* argv[]);
    void Initialize(stringc&& _appname = "X-Day Engine");
    void Destroy();

    static bool isGlobalDebug();
    bool FindParam(stringc param) const;
    string ReturnParam(stringc param) const;

    auto GetBuildId() const { return buildId; }
    auto GetBuildDate() const { return buildDate; }
    auto GetBuildTime() const { return buildTime; }
    auto GetBuildString() const { return buildString; }
    auto GetGLFWVersionString() const { return glfwVersionString; }
    constexpr static pcstr GetBuildConfiguration();

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
