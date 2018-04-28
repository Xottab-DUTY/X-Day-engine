#pragma once

#include "Common/Noncopyable.hpp"

namespace XDay
{
class XDCORE_API ModuleHandle : Noncopyable
{
    cpcstr name;
    void* handle;

public:
    ModuleHandle(cpcstr module);
    ~ModuleHandle();

    auto operator()() const { return handle; }

    auto GetName() const { return name; }
    bool IsLoaded() const { return handle != nullptr; }
    void* GetProcAddress(cpcstr procedure) const;
};

using Module = std::unique_ptr<ModuleHandle>;
inline auto LoadModule(cpcstr module)
{
    return std::make_unique<ModuleHandle>(module);
}
} // namespace XDay
