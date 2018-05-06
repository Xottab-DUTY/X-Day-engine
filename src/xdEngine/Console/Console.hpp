#pragma once

#include <filesystem>
namespace filesystem = std::experimental::filesystem;

namespace XDay::Console
{
class XDENGINE_API CConsole
{
public:
    filesystem::path ConfigFile;

    void Initialize();

    void ExecuteConfig() const { ExecuteConfig(ConfigFile); }
    static void ExecuteConfig(const filesystem::path& path);
};
extern XDENGINE_API CConsole Console;
} // namespace XDay::Console
