#pragma once

#include <filesystem>
namespace filesystem = std::experimental::filesystem;

namespace XDay
{
class XDCORE_API Filesystem
{
    Filesystem() = default;
    static Filesystem instance;

public:
    static void BasicInit();
    static void Initialize();
    static void Destroy();
};

class XDCORE_API CFilesystem
{
public:
    CFilesystem();
    ~CFilesystem();

    void Initialize();
    void Destroy();

    void CreateDirIfNotExist(const filesystem::path& p) const;

    static stringc GetModulePath(string&& initialPath, const bool extension = true);

    // temporary:
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
};
} // namespace XDay

extern XDCORE_API XDay::CFilesystem FS;
