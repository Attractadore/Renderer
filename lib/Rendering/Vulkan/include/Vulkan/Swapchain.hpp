#pragma once
#include "Common/Swapchain.hpp"
#include "Instance.hpp"
#include "Queue.hpp"

namespace R1::VK {
struct SurfaceImpl;
using Surface = SurfaceImpl*;
struct SwapchainImpl;
using Swapchain = SwapchainImpl*;

void DestroySurface(Surface surface);

const SurfaceDescription& GetSurfaceDescription(Surface surface, Device device);
Transform GetSurfaceCurrentTransform(Surface surface, Device device);

using SwapchainConfig = SwapchainConfigBase<Queue>;
Swapchain CreateSwapchain(
    Context ctx,
    Surface surface,
    SurfaceSizeCallback size_cb,
    const SwapchainConfig& config
);
void DestroySwapchain(Swapchain swapchain);

using SwapchainDescription = SwapchainDescriptionBase<Surface>;
const SwapchainDescription& GetSwapchainDescription(Swapchain swapchain);

unsigned GetSwapchainImageCount(Swapchain swapchain);
Image GetSwapchainImage(Swapchain swapchain, unsigned image_idx);

unsigned AcquireImage(
    Swapchain swapchain,
    std::chrono::nanoseconds timeout,
    Semaphore signal_semaphore,
    Fence signal_fence
);

void PresentImage(
    Swapchain swapchain,
    unsigned image_idx,
    std::span<const Semaphore> wait_semaphores
);
};
