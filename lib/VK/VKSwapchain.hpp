#pragma once
#include "VKRAII.hpp"
#include "Common/Types.hpp"

#include <span>
#include <functional>
#include <limits>

namespace R1::VK {
VkSurfaceCapabilitiesKHR getSurfaceCapabilities(
    VkPhysicalDevice pdev, VkSurfaceKHR surf
);

size_t  getSurfaceFormatCount(
    VkPhysicalDevice dev, VkSurfaceKHR surf
);
template <std::contiguous_iterator I>
I       getSurfaceFormats(
    VkPhysicalDevice dev, VkSurfaceKHR surf, I it
);

size_t  getSurfacePresentModeCount(
    VkPhysicalDevice dev, VkSurfaceKHR surf
);
template <std::contiguous_iterator I>
I       getSurfacePresentModes(
    VkPhysicalDevice dev, VkSurfaceKHR surf, I it
);

template <std::contiguous_iterator I>
I getSurfaceFormats(VkPhysicalDevice dev, VkSurfaceKHR surf, I it) {
    auto surf_fmt_cnt = std::numeric_limits<uint32_t>::max();
    auto r = vkGetPhysicalDeviceSurfaceFormatsKHR(
        dev, surf, &surf_fmt_cnt, std::to_address(it)
    );
    if (r) {
        return {};
    };
    return it + surf_fmt_cnt;
}

template <std::contiguous_iterator I>
I getSurfacePresentModes(VkPhysicalDevice dev, VkSurfaceKHR surf, I it) {
    auto pmode_cnt = std::numeric_limits<uint32_t>::max();
    auto r = vkGetPhysicalDeviceSurfacePresentModesKHR(
        dev, surf, &pmode_cnt, std::to_address(it)
    );
    if (r) {
        return {};
    }
    return it + pmode_cnt;
}

struct SwapchainDescription {
    unsigned                        width, height;
    unsigned                        image_count;
    VkImageUsageFlags               image_usage_flags;
    VkSurfaceFormatKHR              surface_format;
    VkSurfaceTransformFlagBitsKHR   surface_pre_transform;
    VkCompositeAlphaFlagBitsKHR     composite_alpha;
    VkPresentModeKHR                present_mode;
};

class Swapchain {
    VkPhysicalDevice                m_physical_device;
    VkSurfaceKHR                    m_surface;
    Vk::Swapchain                   m_swapchain;
    std::vector<VkImage>            m_images;
    unsigned                        m_acquired_count;

    // Retired swapchains can't be deleted until all of their acquired
    // images have been returned
    struct RetiredSwapchain {
        Vk::Swapchain swapchain;
        std::vector<VkImage> images;
        unsigned acquired_count;
    };
    std::vector<RetiredSwapchain> m_retired_swapchains;

    VkSurfaceCapabilitiesKHR        m_capabilities;
    std::vector<VkSurfaceFormatKHR> m_formats;
    std::vector<VkPresentModeKHR>   m_present_modes;
    SwapchainDescription            m_description;
    SizeCallback                    m_size_callback;

public:
    Swapchain(
        VkPhysicalDevice pdev, VkDevice dev, VkSurfaceKHR surf,
        SizeCallback&& size_cb,
        VkImageUsageFlags img_usg_flgs, VkPresentModeKHR pmode
    );

    const auto&         description() const { return m_description; }
    size_t              imageCount() const { return m_images.size(); }
    VkImage             image(size_t i) const { return m_images[i]; }

    using AcquireInfo = std::tuple<VkImage, unsigned, unsigned>;
    AcquireInfo         acquireImage(VkSemaphore sem);

    void                present(VkQueue q, VkImage img, std::span<VkSemaphore> sems);

private:
    RetiredSwapchain    create();
    void                recreate();
};
}
