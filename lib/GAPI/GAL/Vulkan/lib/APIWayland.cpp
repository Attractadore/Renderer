#include "GAL/GALWayland.hpp"
#include "InstanceImpl.hpp"
#include "SwapchainImpl.hpp"
#include "VKUtil.hpp"

#include <vulkan/vulkan_wayland.h>

namespace R1::GAL::Wayland {
Instance CreateInstance(struct wl_display* display, const InstanceConfig& config) {
    static constexpr std::array exts = {
        VK_KHR_SURFACE_EXTENSION_NAME,
        VK_KHR_WAYLAND_SURFACE_EXTENSION_NAME,
    };
    auto instance = CreateVkInstanceWithExtensions(config, exts);
    if (!instance) {
        throw std::runtime_error{"Vulkan: Failed to create Wayland instance"};
    }
    return CreateInstanceFromVK(std::move(instance));
}

Surface CreateSurface(
    Instance instance,
    struct wl_display* display,
    struct wl_surface* surface
) {
    VkWaylandSurfaceCreateInfoKHR create_info = {
        .sType = VK_STRUCTURE_TYPE_WAYLAND_SURFACE_CREATE_INFO_KHR,
        .display = display,
        .surface = surface,
    };
    VkSurfaceKHR surf;
    ThrowIfFailed(vkCreateWaylandSurfaceKHR(
        instance->instance.get(), &create_info, nullptr, &surf),
        "Vulkan: Failed to create Wayland surface");
    return CreateSurfaceFromHandle(instance, {instance->instance.get(), surf});
}
}
