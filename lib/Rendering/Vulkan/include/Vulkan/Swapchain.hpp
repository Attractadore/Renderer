#pragma once
#include "Common/Swapchain.hpp"
#include "Instance.hpp"
#include "Queue.hpp"

#include <optional>

namespace R1 {
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
}

namespace R1::VK {
struct SurfaceImpl;
using Surface = SurfaceImpl*;
struct SwapchainImpl;
using Swapchain = SwapchainImpl*;

void DestroySurface(Surface surface);

const SurfaceDescription& GetSurfaceDescription(
    Surface surface, Device device
);

using SwapchainConfig = SwapchainConfigBase<Queue>;
Swapchain CreateSwapchain(
    Context ctx,
    Surface surface,
    SurfaceSizeCallback size_cb,
    const SwapchainConfig& config
);
void DestroySwapchain(Swapchain swapchain);

std::tuple<unsigned, unsigned> GetSwapchainSize(Swapchain swapchain);
unsigned GetSwapchainImageCount(Swapchain swapchain);
Image GetSwapchainImage(Swapchain swapchain, unsigned image_idx);

std::tuple<unsigned, SwapchainStatus> AcquireImage(
    Swapchain swapchain,
    Semaphore signal_semaphore
);

SwapchainStatus PresentImage(
    Swapchain swapchain,
    unsigned image_idx,
    std::span<const Semaphore> wait_semaphores
);

// Resize the swapchain to match the native window's dimensions
//
// Unless a fast resize is possible, all views to the swapchain's
// images must have been destroyed
void ResizeSwapchain(Swapchain swapchain);
};
