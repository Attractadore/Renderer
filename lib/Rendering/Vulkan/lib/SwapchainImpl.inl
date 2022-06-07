#pragma once

namespace R1::VK {
inline constexpr VkSwapchainCreateFlagsKHR SwapchainCapabilitiesToVK(
    const SwapchainCapabilities& caps
) {
    VkSwapchainCreateFlagsKHR flags = 0;
    flags |= caps.image_mutable_format ? VK_SWAPCHAIN_CREATE_MUTABLE_FORMAT_BIT_KHR: 0;
    return flags;
}
}
