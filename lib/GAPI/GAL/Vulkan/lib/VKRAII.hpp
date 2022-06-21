#pragma once
#include "Common/RAII.hpp"

#include <vk_mem_alloc.h>

namespace R1::GAL::Vk {
namespace Detail {
template<class H>
constexpr int DoRootDeleteF = 0;

struct RootHandleDeleter {
    template<class H>
    static void DoDelete(H handle) noexcept {
        DoRootDeleteF<H>(handle, nullptr);
    }
};

template<class H>
constexpr int DoInstanceDeleteF = 0;

struct InstanceChildHandleDeleter {
    template<class H>
    static void DoDelete(VkInstance instance, H handle) noexcept {
        DoInstanceDeleteF<H>(instance, handle, nullptr);
    }
};

template<class H>
constexpr int DoDeviceDeleteF = 0;

struct DeviceChildHandleDeleter {
    template<class H>
    static void DoDelete(VkDevice device, H handle) noexcept {
        DoDeviceDeleteF<H>(device, handle, nullptr);
    }
};

template<class H>
constexpr int DoAllocatorDeleteF = 0;

struct AllocatorChildHandleDeleter {
    template<class H>
    static void DoDelete(VmaAllocator allocator, H handle) noexcept {
        DoAllocatorDeleteF<H>(allocator, handle);
    }
};

template<class H>
using RootHandle = RootHandle<H, RootHandleDeleter>;

template<class H>
using InstanceHandleBase = ChildHandle<VkInstance, H, InstanceChildHandleDeleter>;
template<class H>
struct InstanceHandle: InstanceHandleBase<H>{
    using InstanceHandleBase<H>::InstanceHandleBase;

    VkInstance get_instance() const noexcept {
        return this->get_deleter().get_parent();
    }
};
}

template<> inline constexpr auto Detail::DoRootDeleteF<VkInstance>          = vkDestroyInstance;
template<> inline constexpr auto Detail::DoRootDeleteF<VkDevice>            = vkDestroyDevice;
template<> inline constexpr auto Detail::DoRootDeleteF<VmaAllocator>        = [] (VmaAllocator alloc, auto) { return vmaDestroyAllocator(alloc); };
using Instance      = Detail::RootHandle<VkInstance>;
using Device        = Detail::RootHandle<VkDevice>;
using Allocator     = Detail::RootHandle<VmaAllocator>;

template<> inline constexpr auto Detail::DoInstanceDeleteF<VkSurfaceKHR>    = vkDestroySurfaceKHR;
using Surface       = Detail::InstanceHandle<VkSurfaceKHR>;
}
