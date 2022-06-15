#pragma once
#include "VulkanQueue.hpp"

namespace R1::GAL {
enum class ColorSpace;

enum class CompositeAlpha {
    Opaque          = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
    PreMultiplied   = VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR,
    PostMultiplied  = VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR,
    Inherit         = VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR,
};

enum class PresentMode {
    Immediate   = VK_PRESENT_MODE_IMMEDIATE_KHR,
    Mailbox     = VK_PRESENT_MODE_MAILBOX_KHR,
    FIFO        = VK_PRESENT_MODE_FIFO_KHR,
    RelaxedFIFO = VK_PRESENT_MODE_FIFO_RELAXED_KHR,
};

struct SurfaceImpl;
using Surface = SurfaceImpl*;
struct SwapchainImpl;
using Swapchain = SwapchainImpl*;
};
