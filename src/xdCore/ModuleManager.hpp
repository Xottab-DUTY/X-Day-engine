#pragma once

namespace XDay
{
enum EngineModules
{
    eAPIModule,
    eCoreModule,
    eEngineModule,
    eMainModule,
    eRendererModule
};
class XDCORE_API Module
{
    pcstr name;
    void* handle;

public:
    Module(pcstr moduleName);
    Module(const Module&) = delete;
    ~Module();

    void* operator()() const { return handle; }

    pcstr GetName() const { return name; }
    bool NotEmpty() const { return handle != nullptr; }
    void* Load(pcstr procName) const;
};

class XDCORE_API ModuleManager
{
    //ModuleManager();
    //~ModuleManager();

    std::vector<std::shared_ptr<Module>> modules;

public:
    static ModuleManager instance;

    static std::shared_ptr<Module> GetModule(const EngineModules xdModule);
    static void LoadModule(EngineModules xdModule);
    static void* GetProcFromModule(EngineModules xdModule, pcstr procName);

    static std::string GetModuleName(EngineModules xdModule, const bool needExt = true);
    static std::string GetModuleName(std::string&& xdModule, const bool needExt, bool isExecutable = false);
    static std::string GetModuleExtension(std::string&& xdModule, bool isExecutable = false);
};
} // namespace XDay
