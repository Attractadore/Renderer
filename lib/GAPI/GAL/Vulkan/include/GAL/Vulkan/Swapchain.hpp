#pragma once
#include "GAL/Image.hpp"
#include "VulkanQueue.hpp"

#include <functional>

namespace R1::GAL::Vulkan {
struct SwapchainImpl;
using Swapchain = SwapchainImpl*;

using SurfaceSizeCallback = std::function<std::tuple<unsigned, unsigned>()>;

struct SurfaceDescription {
    std::vector<VkSurfaceFormatKHR> supported_formats;
    std::vector<VkPresentModeKHR>   supported_present_modes;
    VkCompositeAlphaFlagsKHR        supported_composite_alpha;
    unsigned                        min_image_count;
    unsigned                        max_image_count;
    VkImageUsageFlags               supported_image_usage;
};

struct SwapchainConfig {
    VkFormat                        format;
    VkColorSpaceKHR                 color_space;
    VkImageUsageFlags               image_usage;
    unsigned                        image_count;
    VkCompositeAlphaFlagBitsKHR     composite_alpha;
    VkPresentModeKHR                present_mode;
    bool                            clipped: 1;
};

enum class SwapchainStatus {
    Optimal,
    OutOfDate,
};

SurfaceDescription GetSurfaceDescription(
    VkSurfaceKHR surface, Device device
);

Swapchain CreateSwapchain(
    Context ctx,
    VkSurfaceKHR surface,
    SurfaceSizeCallback size_cb,
    const SwapchainConfig& config
);
void DestroySwapchain(Swapchain swapchain);

std::tuple<unsigned, unsigned> GetSwapchainSize(Swapchain swapchain);
unsigned GetSwapchainImageCount(Swapchain swapchain);
Image GetSwapchainImage(Swapchain swapchain, size_t idx);

std::tuple<unsigned, SwapchainStatus> AcquireImage(
    Swapchain swapchain,
    VkSemaphore signal_semaphore
);

SwapchainStatus PresentImage(
    VkQueue queue,
    Swapchain swapchain,
    unsigned image_idx,
    VkSemaphore wait_semaphore
);

// Resize the swapchain to match the native window's dimensions.
// All views of the swapchain's images must have been destroyed.
void ResizeSwapchain(Swapchain swapchain);
};
