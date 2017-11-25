#include "pch.hpp"
#include "ModuleManager.hpp"

#ifdef LINUX
#include <dlfcn.h>
#endif

XDay::ModuleManager XDay::ModuleManager::instance;

namespace XDay
{
Module::Module(pcstr moduleName) : name(moduleName), handle(nullptr)
{
    Log::Info("Loading module: {}", name);

#ifdef WINDOWS
    handle = static_cast<void*>(LoadLibraryA(name));
#else
    handle = dlopen(name, RTLD_LAZY);
#endif

    if (handle == nullptr)
    {
        Log::Error("Failed to load module: {}", name);
#ifdef LINUX
        Log::Error(dlerror());
#endif
    }
}

Module::~Module()
{
    bool closed = false;
#ifdef WINDOWS
    closed = FreeLibrary(static_cast<HMODULE>(handle)) != 0;
#else
    closed = dlclose(handle) == 0;
#endif

    if (closed == false)
    {
        Log::Error("Failed to close module: {}", name);
#ifdef LINUX
        Log::Error(dlerror());
#endif
}
}

void* Module::Load(pcstr procName) const
{
    void* proc = nullptr;

#ifdef WINDOWS
    proc = GetProcAddress(static_cast<HMODULE>(handle), procName);
#else
    proc = dlsym(handle, sym)
#endif

    if (proc == nullptr)
    {
        Log::Error("Failed to load procedure [{}] from module: {}", procName, name);
#ifdef LINUX
        Log::Error(dlerror());
#endif
    }

    return proc;
}

// Returns module name with configuration, architecture and extension
std::string ModuleManager::GetModuleName(const EngineModules xdModule, const bool needExt)
{
    switch (xdModule)
    {
    case eAPIModule:
        return GetModuleName("X-Day.API", needExt);
    case eCoreModule:
        return GetModuleName("X-Day.Core", needExt);
    case eEngineModule:
        return GetModuleName("X-Day.Engine", needExt);
    case eMainModule:
        return GetModuleName("X-Day", needExt, true);
    case eRendererModule:
        return GetModuleName("X-Day.Renderer", needExt);
    default:
        throw "Create the case for the module here!";
    }
}

// Returns given module name with configuration and architecture and extension
std::string ModuleManager::GetModuleName(std::string&& xdModule, const bool needExt, const bool isExecutable)
{
#ifdef DEBUG
#ifdef XD_X64
    if (needExt)
        return GetModuleExtension(xdModule + ".Dx64", isExecutable);
    return xdModule + ".Dx64";
#else
    if (needExt)
        return GetModuleExtension(xdModule + ".Dx86", isExecutable);
    return xdModule + ".Dx86";
#endif
#elif NDEBUG
#ifdef XD_X64
    if (needExt)
        return GetModuleExtension(xdModule + ".Rx64", isExecutable);
    return xdModule + ".Rx64";
#else
    if (needExt)
        return GetModuleExtension(xdModule + ".Rx86", isExecutable);
    return xdModule + ".Rx86";
#endif
#endif
}

// Returns given module name with extension
std::string ModuleManager::GetModuleExtension(std::string&& xdModule, const bool isExecutable)
{
#ifdef WINDOWS
    if (isExecutable)
        return xdModule + ".exe";
    return xdModule + ".dll";
#elif defined(LINUX)
    if (isExecutable)
        return xdModule;
    return xdModule + ".so";
#endif
}

std::shared_ptr<Module> ModuleManager::GetModule(const EngineModules xdModule)
{
    auto moduleName = GetModuleName(xdModule);
    for (const auto _module : instance.modules)
        if (moduleName.compare(_module->GetName()) == 0)
            return _module;
    return nullptr;
}

void ModuleManager::LoadModule(const EngineModules xdModule)
{
    if (GetModule(xdModule) != nullptr)
        return;

    auto new_module = std::make_shared<Module>(GetModuleName(xdModule).c_str());
    instance.modules.emplace_back(std::move(new_module));
}

void* ModuleManager::GetProcFromModule(const EngineModules xdModule, pcstr procName)
{
    const auto _module = GetModule(xdModule);
    if (_module != nullptr)
        return _module->Load(procName);
    return nullptr;
}
} // namespace XDay
