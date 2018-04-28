#include "pch.hpp"
#include "Core.hpp"
#include "CommandLine/Keys.hpp"
#include "Filesystem.hpp"
#include "XML/XMLResource.hpp"

XDCORE_API XDay::Filesystem FS;

namespace XDay
{
Filesystem::Filesystem() {}
Filesystem::~Filesystem() {}

void Filesystem::Initialize()
{
    AppPath = filesystem::absolute(Core.GetParams().front());
    WorkPath = filesystem::current_path();
    BinPath = WorkPath.string() + "/bin/";

    auto& key = CommandLine::KeyDataPath;
    key.IsSet() ? DataPath = key.StringValue() : DataPath = WorkPath.string() + "/appdata/";

    key = CommandLine::KeyResPath;
    key.IsSet() ? ResourcesPath = key.StringValue() : ResourcesPath = WorkPath.string() + "/resources/";

    BinaryShadersPath = DataPath.string() + "/binary_shaders/";
    LogsPath = DataPath.string() + "/logs/";
    SavesPath = DataPath.string() + "/saves/";

    InitializeResources();

    CreateDirIfNotExist(DataPath);
    CreateDirIfNotExist(BinaryShadersPath);
    CreateDirIfNotExist(LogsPath);
    CreateDirIfNotExist(SavesPath);

    CreateDirIfNotExist(ResourcesPath);
    CreateDirIfNotExist(ArchivesPath);
    CreateDirIfNotExist(ConfigsPath);
    CreateDirIfNotExist(ModelsPath);
    CreateDirIfNotExist(ShadersPath);
    CreateDirIfNotExist(SoundsPath);
    CreateDirIfNotExist(TexturesPath);
}

void Filesystem::InitializeResources()
{
    ArchivesPath = ResourcesPath.string() + "/archives/";
    ConfigsPath = ResourcesPath.string() + "/configs/";
    ModelsPath = ResourcesPath.string() + "/models/";
    ShadersPath = ResourcesPath.string() + "/shaders/";
    SoundsPath = ResourcesPath.string() + "/sounds/";
    TexturesPath = ResourcesPath.string() + "/textures/";

    xdXMLResource resource_initializer(ResourcesPath, "resources.xml");
    if (!resource_initializer.isErrored())
        resource_initializer.ParseResources();
}

void Filesystem::CreateDirIfNotExist(const filesystem::path& p) const
{
    if (!filesystem::exists(p)) filesystem::create_directory(p);
}

stringc Filesystem::GetModulePath(string&& initialPath, const bool extension)
{
#ifdef DEBUG
#ifdef XD_X64
    if (extension)
        return initialPath + ".Dx64.dll";
    return initialPath + ".Dx64";
#else
    if (extension)
        return initialPath + ".Dx86.dll";
    return initialPath + ".Dx86";
#endif
#elif NDEBUG
#ifdef XD_X64
    if (extension)
        return initialPath + ".Rx64.dll";
    return initialPath + ".Rx64";
#else
    if (extension)
        return initialPath + ".Rx86.dll";
    return initialPath + ".Rx86";
#endif
#endif
}
} // namespace XDay
