#pragma once
#include "Common/Types.hpp"

#include <vulkan/vulkan.h>

#include <cassert>

namespace R1::VK {
class Instance;
class Context;
class Swapchain;

struct QueueFamilies {
    static constexpr int32_t NotFound = -1; 
    int32_t graphics;
};

struct Queues {
    VkQueue graphics;
};

constexpr VkPresentModeKHR PresentModeToVK(PresentMode pmode) {
    using enum PresentMode;
    switch (pmode) {
        case NoVSync:
            return VK_PRESENT_MODE_IMMEDIATE_KHR;
        case VSync:
            return VK_PRESENT_MODE_FIFO_KHR;
        case AdaptiveVSync:
            return VK_PRESENT_MODE_FIFO_RELAXED_KHR;
        case MultibufferedVSync:
            return VK_PRESENT_MODE_MAILBOX_KHR;
        default:
            assert(!"Vulkan: Unknown present mode");
    }
}

constexpr PresentMode PresentModeFromVK(VkPresentModeKHR pmode) {
    using enum PresentMode;
    switch(pmode) {
        case VK_PRESENT_MODE_IMMEDIATE_KHR:
            return NoVSync;
        case VK_PRESENT_MODE_FIFO_KHR:
            return VSync;
        case VK_PRESENT_MODE_FIFO_RELAXED_KHR:
            return AdaptiveVSync;
        case VK_PRESENT_MODE_MAILBOX_KHR:
            return MultibufferedVSync;
        default:
            assert(!"Vulkan: Unknown present mode");
    }
}
}
