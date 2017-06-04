#pragma once
#ifndef VDeleter_hpp__
#define VDeleter_hpp__
#include <functional>

#include <vulkan/vulkan.hpp>


namespace vk
{
template <typename T>
class VDeleter
{
public:
    //VDeleter() : VDeleter() {}

    /*VDeleter(std::function<void(T, Optional<const AllocationCallbacks*> pAllocator)> deletef)
    {
        this->deleter = [=](T obj, Optional<const AllocationCallbacks*> pAllocator) { deletef(obj, pAllocator); };
    }*/

    /*VDeleter(const VDeleter<Instance>& instance, std::function<void(Instance, T, Optional<const AllocationCallbacks*>)> deletef)
    {
        this->deleter = [&instance, deletef](T obj) { deletef(instance, obj, nullptr); };
    }*/

    /*VDeleter(const VDeleter<Device>& device, std::function<void(Device, T, Optional<const AllocationCallbacks*>)> deletef)
    {
        this->deleter = [&device, deletef](T obj) { deletef(device, obj, nullptr); };
    }*/

    ~VDeleter()
    {
        cleanup();
    }

    void setDeleter(std::function<void(Optional<const AllocationCallbacks*> pAllocator)> deletef)
    {
        this->deleter = [=](T obj, Optional<const AllocationCallbacks*> pAllocator) { obj->deletef(pAllocator); };
    }

    const T* operator &() const
    {
        return &object;
    }

    T* replace()
    {
        cleanup();
        return &object;
    }

    operator T() const
    {
        return object;
    }

    void operator=(T rhs)
    {
        if (rhs != object)
        {
            cleanup();
            object = rhs;
        }
    }

    template <typename V>
    bool operator==(V rhs)
    {
        return object == T(rhs);
    }

    T* operator->() const
    {
        return object;
    }

    T getObject() const
    {
        return object;
    }

private:
    T object{nullptr};
    std::function<void(T)> deleter;

    void cleanup()
    {
        if (static_cast<T>(object) != static_cast<T>(nullptr))
            deleter(object);
        object = nullptr;
    }
};
}
#endif // VDeleter_hpp__
