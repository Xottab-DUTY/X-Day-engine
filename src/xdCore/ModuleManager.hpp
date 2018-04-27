#pragma once

namespace XDay
{
class XDCORE_API ModuleHandle
{
    cpcstr name;
    void* handle;

public:
    ModuleHandle(cpcstr moduleName);
    ModuleHandle(const ModuleHandle&) = delete;
    ~ModuleHandle();

    auto operator()() const { return handle; }

    auto GetName() const { return name; }
    bool NotEmpty() const { return handle != nullptr; }
    void* getProcAddress(cpcstr procName) const;
};

using Module = std::unique_ptr<ModuleHandle>;

// XXX: Do something with all the ModuleManager stuff: refactor, rewrite or remove it
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

    vector<std::shared_ptr<ModuleHandle>> modules;

public:
    static ModuleManager instance;

    static std::shared_ptr<ModuleHandle> GetModule(const EngineModules xdModule);
    static void LoadModule(EngineModules xdModule);
    static void* GetProcFromModule(EngineModules xdModule, cpcstr procName);

    static stringc GetModuleName(EngineModules xdModule, const bool needExt = true);
    static stringc GetModuleName(stringc&& xdModule, const bool needExt, bool isExecutable = false);
    static stringc GetModuleExtension(stringc&& xdModule, bool isExecutable = false);
};
} // namespace XDay
