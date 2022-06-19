#pragma once
#if GAL_USE_VULKAN
#include "VulkanSwapchain.hpp"
#endif

#include "Image.hpp"
#include "Sync.hpp"

#include <functional>

namespace R1::GAL {
namespace Detail {
template<typename E>
concept IsColorSpace = requires(E e) {
    true;
};

template<typename E>
concept IsCompositeAlpha = requires(E e) {
    E::Opaque;
    E::PreMultiplied;
    E::PostMultiplied;
    E::Inherit;
};

template<typename E>
concept IsPresentMode = requires(E e) {
    E::Immediate;
    E::Mailbox;
    E::FIFO;
    E::RelaxedFIFO;
};

static_assert(IsColorSpace<ColorSpace>);
static_assert(IsCompositeAlpha<CompositeAlpha>);
static_assert(IsPresentMode<PresentMode>);
}

using SurfaceSizeCallback = std::function<std::tuple<unsigned, unsigned>()>;

struct SurfaceFormat {
    Format      format;
    ColorSpace  color_space;
};

struct SurfaceDescription {
    std::vector<SurfaceFormat>  supported_formats;
    std::vector<PresentMode>    supported_present_modes;
    std::vector<CompositeAlpha> supported_composite_alphas;
    unsigned                    min_image_count;
    unsigned                    max_image_count;
    ImageUsageFlags             supported_image_usage;
};

struct SwapchainConfig {
    Format                              format;
    ColorSpace                          color_space;
    ImageUsageFlags                     image_usage;
    unsigned                            image_count;
    std::span<const QueueFamily::ID>    image_sharing_queue_families;
    CompositeAlpha                      composite_alpha;
    PresentMode                         present_mode;
    Queue                               present_queue;
    bool                                clipped: 1;
};

enum class SwapchainStatus {
    Optimal,
    OutOfDate,
};

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
