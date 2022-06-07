#pragma once
#include "Common/Handle.hpp"
#include "VKUtil.hpp"

#include <vulkan/vulkan.h>
#include <X11/Xlib.h>
#include <xcb/xcb.h>
#include <vulkan/vulkan_xlib.h>
#include <vulkan/vulkan_xcb.h>

#include <cassert>

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

template<> struct InstanceDestroyFunction<VkSurfaceKHR> {
    static constexpr auto function = vkDestroySurfaceKHR;
};

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

template<> struct DeviceDestroyFunction<VkSwapchainKHR> {
    static constexpr auto function = vkDestroySwapchainKHR;
};

template<> struct DeviceDestroyFunction<VkPipelineLayout> {
    static constexpr auto function = vkDestroyPipelineLayout;
};

template<> struct DeviceDestroyFunction<VkShaderModule> {
    static constexpr auto function = vkDestroyShaderModule;
};

template<> struct DeviceDestroyFunction<VkPipeline> {
    static constexpr auto function = vkDestroyPipeline;
};

template<> struct DeviceDestroyFunction<VkFence> {
    static constexpr auto function = vkDestroyFence;
};

template<> struct DeviceDestroyFunction<VkSemaphore> {
    static constexpr auto function = vkDestroySemaphore;
};

template<> struct DeviceDestroyFunction<VkImage> {
    static constexpr auto function = vkDestroyImage;
};

template<> struct DeviceDestroyFunction<VkImageView> {
    static constexpr auto function = vkDestroyImageView;
};

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
using ParentHandleBase = Handle<VulkanHandle, Detail::HandleDeleter>;

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
using WithParentHandleBase = Handle<VulkanHandle, Detail::WithParentHandleDeleter<ParentVulkanHandle>>;

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

using Instance  = Detail::ParentHandle<VkInstance>;
using Device    = Detail::ParentHandle<VkDevice>;

namespace Detail {
using SurfaceBase = WithInstanceHandle<VkSurfaceKHR>;
}

class Surface: public Detail::SurfaceBase {
public:
    Surface(VkInstance instance, ::Display* dpy, ::Window win):
    Detail::SurfaceBase {
        instance,
        [&] {
            VkXlibSurfaceCreateInfoKHR create_info = {
                .sType = sType(create_info),
                .dpy = dpy,
                .window = win,
            };
            VkSurfaceKHR surf;
            auto r = vkCreateXlibSurfaceKHR(instance, &create_info, nullptr, &surf);
            if (r) {
                throw std::runtime_error{"Failed to create Xlib surface"};
            }
            return surf;
        } ()
    } {}

    Surface(VkInstance instance, xcb_connection_t* con, xcb_window_t win):
    Detail::SurfaceBase {
        instance,
        [&] {
            VkXcbSurfaceCreateInfoKHR create_info = {
                .sType = sType(create_info),
                .connection = con,
                .window = win,
            };
            VkSurfaceKHR surf;
            auto r = vkCreateXcbSurfaceKHR(instance, &create_info, nullptr, &surf);
            if (r) {
                throw std::runtime_error{"Failed to create XCB surface"};
            }
            return surf;
        } ()
    } {}
};

using Swapchain         = Detail::WithDeviceHandle<VkSwapchainKHR>;
using PipelineLayout    = Detail::WithDeviceHandle<VkPipelineLayout>;
using ShaderModule      = Detail::WithDeviceHandle<VkShaderModule>;
using Pipeline          = Detail::WithDeviceHandle<VkPipeline>;
using Fence             = Detail::WithDeviceHandle<VkFence>;
using Semaphore         = Detail::WithDeviceHandle<VkSemaphore>;
using Image             = Detail::WithDeviceHandle<VkImage>;
using ImageView         = Detail::WithDeviceHandle<VkImageView>;
}
