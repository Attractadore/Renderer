#include "APIXCB.hpp"
#include "Internal.hpp"

#include <vulkan/vulkan_xcb.h>

namespace R1::VK {
Instance CreateInstanceXCB(::xcb_connection_t* c, const InstanceConfig& config, int screen) {
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
}
