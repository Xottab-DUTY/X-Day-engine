#include "pch.hpp"
#include "Module.hpp"

#ifdef LINUX
#include <dlfcn.h>
#endif

namespace XDay
{
ModuleHandle::ModuleHandle(cpcstr module) : name(module), handle(nullptr)
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

ModuleHandle::~ModuleHandle()
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
#ifdef WINDOWS
        Log::Error(std::to_string(GetLastError()));
#elif LINUX
        Log::Error(dlerror());
#endif
    }
}

void* ModuleHandle::GetProcAddress(cpcstr procedure) const
{
    void* proc = nullptr;

#ifdef WINDOWS
    proc = ::GetProcAddress(static_cast<HMODULE>(handle), procedure);
#else
    proc = dlsym(handle, procedure)
#endif

    if (proc == nullptr)
    {
        Log::Error("Failed to load procedure [{}] from module: {}", procedure, name);
#ifdef WINDOWS
        Log::Error(std::to_string(GetLastError()));
#elif LINUX
        Log::Error(dlerror());
#endif
    }

    return proc;
}
} // namespace XDay
