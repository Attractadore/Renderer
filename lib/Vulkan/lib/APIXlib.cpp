#include "APIXlib.hpp"
#include "Internal.hpp"

#include <vulkan/vulkan_xlib.h>

namespace R1::VK {
Instance CreateInstanceXlib(::Display* dpy, const InstanceConfig& config, int screen) {
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
}
