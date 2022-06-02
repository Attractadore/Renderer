#pragma once
#include "Common/Handle.hpp"

#include <cassert>

#include <vulkan/vulkan.h>

namespace R1::VK::Vk {
namespace Detail {
struct HandleDeleter {
    void operator()(VkInstance instance) const {
        vkDestroyInstance(instance, nullptr);
    }

    void operator()(VkDevice dev) const {
        vkDeviceWaitIdle(dev);
        vkDestroyDevice(dev, nullptr);
    }
};

template<class ParentVulkanHandle>
class WithParentHandleDeleter;

template<class VulkanHandle>
struct InstanceDestroyFunction;

template<class VulkanHandle>
static constexpr auto InstanceDestroyFunctionV =
    InstanceDestroyFunction<VulkanHandle>::function;

template<>
class WithParentHandleDeleter<VkInstance> {
    VkInstance m_instance;

public:
    explicit WithParentHandleDeleter(VkInstance instance):
        m_instance{instance} {
        assert(m_instance);
    }

    VkInstance get_instance() const { return m_instance; }

    template<class VulkanHandle>
    void operator()(VulkanHandle handle) const {
        InstanceDestroyFunctionV<VulkanHandle>(get_instance(), handle, nullptr);
    }
};

template<class VulkanHandle>
struct DeviceDestroyFunction;

template<class VulkanHandle>
static constexpr auto DeviceDestroyFunctionV =
    DeviceDestroyFunction<VulkanHandle>::function;

template<>
class WithParentHandleDeleter<VkDevice> {
    VkDevice m_device;

public:
    explicit WithParentHandleDeleter(VkDevice dev):
        m_device{dev} {
        assert(m_device);
    }

    VkDevice get_device() const { return m_device; }

    template<class VulkanHandle>
    void operator()(VulkanHandle handle) const {
        DeviceDestroyFunctionV<VulkanHandle>(get_device(), handle, nullptr);
    }
};

template<class VulkanHandle>
using ParentHandleBase =
    Handle<VulkanHandle, Detail::HandleDeleter>;

template<class VulkanHandle>
class ParentHandle: ParentHandleBase<VulkanHandle> {
    using Base = ParentHandleBase<VulkanHandle>;

public:
    explicit ParentHandle(VulkanHandle handle):
        Base{handle} {}

    using Base::get;
    using Base::operator bool;
};

template<class ParentVulkanHandle, class VulkanHandle>
using WithParentHandleBase =
    Handle<VulkanHandle, Detail::WithParentHandleDeleter<ParentVulkanHandle>>;

template<class ParentVulkanHandle, class VulkanHandle>
class WithParentHandle: WithParentHandleBase<ParentVulkanHandle, VulkanHandle> {
    using Base = WithParentHandleBase<ParentVulkanHandle, VulkanHandle>;

public:
    explicit WithParentHandle(ParentVulkanHandle phandle, VulkanHandle handle):
        Base{handle, Detail::WithParentHandleDeleter<ParentVulkanHandle>{phandle}} {}

    using Base::get;
    using Base::reset;
    using Base::release;
    using Base::operator bool;

protected:
    using Base::get_deleter;
};

template<class VulkanHandle>
class WithInstanceHandle: public WithParentHandle<VkInstance, VulkanHandle> {
    using Base = WithParentHandle<VkInstance, VulkanHandle>;

public:
    using Base::Base;

    VkInstance get_instance() const {
        return this->get_deleter().get_instance();
    }
};

template<class VulkanHandle>
class WithDeviceHandle: public WithParentHandle<VkDevice, VulkanHandle> {
    using Base = WithParentHandle<VkDevice, VulkanHandle>;

public:
    using Base::Base;

    VkDevice get_device() const {
        return this->get_deleter().get_device();
    }
};
}

using Instance = Detail::ParentHandle<VkInstance>;
}
