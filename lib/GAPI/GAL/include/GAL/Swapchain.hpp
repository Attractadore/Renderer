#pragma once
#include "Types.hpp"

namespace R1::GAL {
void DestroySurface(Surface surface);

const SurfaceDescription& GetSurfaceDescription(
    Surface surface, Device device
);

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
