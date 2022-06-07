#include "APIXlib.hpp"
#include "InstanceImpl.hpp"

#include <vulkan/vulkan_xlib.h>

namespace R1::VK {
Instance CreateInstanceXlib(
    ::Display* dpy,
    int screen,
    const InstanceConfig& config
) {
    static constexpr std::array exts = {
        VK_KHR_SURFACE_EXTENSION_NAME,
        VK_KHR_XLIB_SURFACE_EXTENSION_NAME,
    };
    auto instance = CreateVkInstanceWithExtensions(config, exts);
    if (!instance) {
        throw std::runtime_error{"Vulkan: Failed to create Xlib instance"};
    }
    return CreateInstanceFromVK(std::move(instance));
}

Surface CreateSurfaceXlib(
    Instance instance,
    ::Display* dpy,
    ::Window window
) {
    VkXlibSurfaceCreateInfoKHR create_info = {
        .sType = VK_STRUCTURE_TYPE_XLIB_SURFACE_CREATE_INFO_KHR,
        .dpy = dpy,
        .window = window,
    };
    VkSurfaceKHR surface = VK_NULL_HANDLE;
    vkCreateXlibSurfaceKHR(
        instance->instance.get(), &create_info, nullptr, &surface);
    if (!surface) {
        throw std::runtime_error{"Vulkan: Failed to create Xlib surface"};
    }
    return surface;
}
}
