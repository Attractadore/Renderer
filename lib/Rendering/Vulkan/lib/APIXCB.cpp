#include "APIXCB.hpp"
#include "InstanceImpl.hpp"
#include "SwapchainImpl.hpp"

#include <vulkan/vulkan_xcb.h>

namespace R1::VK {
Instance CreateInstanceXCB(::xcb_connection_t* c, int screen, const InstanceConfig& config) {
    static constexpr std::array exts = {
        VK_KHR_SURFACE_EXTENSION_NAME,
        VK_KHR_XCB_SURFACE_EXTENSION_NAME,
    };
    auto instance = CreateVkInstanceWithExtensions(config, exts);
    if (!instance) {
        throw std::runtime_error{"Vulkan: Failed to create XCB instance"};
    }
    return CreateInstanceFromVK(std::move(instance));
}

Surface CreateSurfaceXCB(
    Instance instance,
    xcb_connection_t* conn,
    xcb_window_t window
) {
    VkXcbSurfaceCreateInfoKHR create_info = {
        .sType = VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR,
        .connection = conn,
        .window = window,
    };
    VkSurfaceKHR surface = VK_NULL_HANDLE;
    vkCreateXcbSurfaceKHR(
        instance->instance.get(), &create_info, nullptr, &surface);
    Vk::Surface handle{instance->instance.get(), surface};
    if (!handle) {
        throw std::runtime_error{"Vulkan: Failed to create XCB surface"};
    }
    return CreateSurfaceFromHandle(instance, std::move(handle));
}
}
