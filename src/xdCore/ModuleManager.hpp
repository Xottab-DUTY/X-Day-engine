#pragma once

namespace XDay
{
class XDCORE_API ModuleHandle
{
    const char* const name;
    void* handle;

public:
    ModuleHandle(const char* moduleName);
    ModuleHandle(const ModuleHandle&) = delete;
    ~ModuleHandle();

    void* operator()() const { return handle; }

    const char* GetName() const { return name; }
    bool NotEmpty() const { return handle != nullptr; }
    void* getProcAddress(const char* procName) const;
};

using Module = std::unique_ptr<ModuleHandle>;

enum class EngineModules
{
    API,
    Core,
    Engine,
    Main,
    Renderer
};

class XDCORE_API ModuleManager
{
    //ModuleManager();
    //~ModuleManager();

    std::vector<std::shared_ptr<ModuleHandle>> modules;

public:
    static ModuleManager instance;

    static std::shared_ptr<ModuleHandle> GetModule(const EngineModules xdModule);
    static void LoadModule(EngineModules xdModule);
    static void* GetProcFromModule(EngineModules xdModule, const char* procName);

    static std::string GetModuleName(EngineModules xdModule, const bool needExt = true);
    static std::string GetModuleName(std::string&& xdModule, const bool needExt, bool isExecutable = false);
    static std::string GetModuleExtension(std::string&& xdModule, bool isExecutable = false);
};
} // namespace XDay
