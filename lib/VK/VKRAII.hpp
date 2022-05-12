#pragma once
#include "Common/Util.hpp"
#include "VKUtil.hpp"

#include <vulkan/vulkan.h>
#include <X11/Xlib.h>
#include <xcb/xcb.h>
#include <vulkan/vulkan_xlib.h>
#include <vulkan/vulkan_xcb.h>

#include <cassert>

namespace R1::VK::Vk {
namespace Detail {
class WithInstanceDeleter {
    VkInstance m_instance;

public:
    explicit WithInstanceDeleter(VkInstance instance):
        m_instance{instance} {
        assert(m_instance);
    }

    VkInstance get_instance() const { return m_instance; }
};

class WithDeviceDeleter {
    VkDevice m_device;

public:
    explicit WithDeviceDeleter(VkDevice dev):
        m_device{dev} {
        assert(m_device);
    }

    VkDevice get_device() const { return m_device; }
};

struct InstanceDeleter {
    void operator()(VkInstance instance) const {
        vkDestroyInstance(instance, nullptr);
    }
};

struct SurfaceDeleter: public WithInstanceDeleter {
    using WithInstanceDeleter::WithInstanceDeleter;
    void operator()(VkSurfaceKHR surf) {
        vkDestroySurfaceKHR(get_instance(), surf, nullptr);
    }
};

struct DeviceDeleter {
    void operator()(VkDevice dev) const {
        vkDeviceWaitIdle(dev);
        vkDestroyDevice(dev, nullptr);
    }
};

struct SwapchainDeleter: WithDeviceDeleter {
    using WithDeviceDeleter::WithDeviceDeleter;
    void operator()(VkSwapchainKHR swc) const {
        vkDestroySwapchainKHR(get_device(), swc, nullptr);
    }
};

using InstanceBase  = Handle<VkInstance, InstanceDeleter>;
using SurfaceBase   = Handle<VkSurfaceKHR, SurfaceDeleter>;
using DeviceBase    = Handle<VkDevice, DeviceDeleter>;
using SwapchainBase = Handle<VkSwapchainKHR, SwapchainDeleter>;
}

class Instance: Detail::InstanceBase {
public:
    explicit Instance(VkInstance instance):
        Detail::InstanceBase{instance} {}

    using Detail::InstanceBase::get;
    using Detail::InstanceBase::operator bool;
};

class Surface: Detail::SurfaceBase {
public:
    Surface(VkInstance instance, ::Display* dpy, ::Window win):
    Detail::SurfaceBase {
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
        } (),
        Detail::SurfaceDeleter{instance}
    } {}

    Surface(VkInstance instance, xcb_connection_t* con, xcb_window_t win):
    Detail::SurfaceBase {
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
        } (),
        Detail::SurfaceDeleter{instance}
    } {}

    using Detail::SurfaceBase::get;
};

class Device: Detail::DeviceBase {
public:
    explicit Device(VkDevice dev):
        Detail::DeviceBase{dev} {}

    using Detail::DeviceBase::get;
    using Detail::DeviceBase::operator bool;
};

class Swapchain: Detail::SwapchainBase {
public:
    Swapchain(VkDevice dev, VkSwapchainKHR swc):
        Detail::SwapchainBase{swc, Detail::SwapchainDeleter{dev}} {}

    using Detail::SwapchainBase::get;
    using Detail::SwapchainBase::reset;

    VkDevice get_device() const {
        return get_deleter().get_device();
    }
};
}
