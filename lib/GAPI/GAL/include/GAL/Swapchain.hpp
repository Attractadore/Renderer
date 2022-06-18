#pragma once
#include "SwapchainCommon.hpp"
#include "Sync.hpp"
#include "Traits.hpp"

namespace R1::GAL {
using SurfaceFormat = Detail::SurfaceFormatBase<Detail::Traits>;
using SurfaceDescription = Detail::SurfaceDescriptionBase<Detail::Traits>;
using SwapchainConfig = Detail::SwapchainConfigBase<Detail::Traits>;

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
    const SemaphoreState* signal_state
);

SwapchainStatus PresentImage(
    Swapchain swapchain,
    unsigned image_idx,
    std::span<const SemaphoreState> wait_states,
    const SemaphoreState* signal_state
);

// Resize the swapchain to match the native window's dimensions.
// All views of the swapchain's images must have been destroyed.
void ResizeSwapchain(Swapchain swapchain);
};
