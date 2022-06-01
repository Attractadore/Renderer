#pragma once
#include "Common/Types.hpp"
#include "VKImage.hpp"
#include "VKSync.hpp"

#include <span>
#include <functional>
#include <limits>

#include <iostream>

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
        return it;
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
        return it;
    }
    return it + pmode_cnt;
}

namespace Detail {
struct SwapchainCapabilites {
    Image::Usage                image_usage;
    std::vector<PresentMode>    present_modes;
};

struct SwapchainDescription {
    Image::Description  image_description;
    unsigned            image_count;
    PresentMode         present_mode;
};
}

class Swapchain {
public:
    using Capabilities = Detail::SwapchainCapabilites;
    using Description = Detail::SwapchainDescription;

private:
    VkPhysicalDevice                m_physical_device;

    VkSurfaceKHR                    m_surface;

    SizeCallback                    m_size_callback;

    Vk::Swapchain                   m_swapchain;
    Capabilities                    m_capabilities;
    Description                     m_description;
    // Swapchain images are owned by the swapchain and
    // must not be destroyed
    struct SwapchainImageRef: ImageRef {
        SwapchainImageRef() = default;
        SwapchainImageRef(ImageRef r):
            ImageRef{std::move(r)} {}
        SwapchainImageRef(const SwapchainImageRef&) = delete;
        SwapchainImageRef(SwapchainImageRef&&) = default;
        SwapchainImageRef& operator=(const SwapchainImageRef&) = delete;
        SwapchainImageRef& operator=(SwapchainImageRef&&) = default;
        ~SwapchainImageRef() {
            if (auto p = get()) {
                (void) p->m_image.release();
            }
        }
    };
    std::vector<SwapchainImageRef>  m_images;
    std::vector<SemaphoreRef>       m_signal_semaphores;
    std::vector<SemaphoreRef>       m_present_semaphores;
    unsigned                        m_acquired_count;

    // Retired swapchains can't be deleted until all of their acquired
    // images have been returned
    struct RetiredSwapchain {
        Vk::Swapchain swapchain;
        std::vector<SwapchainImageRef> images;
        std::vector<SemaphoreRef> signal_semaphores;
        std::vector<SemaphoreRef> present_semaphores;
        unsigned acquired_count;
    };
    std::vector<RetiredSwapchain>           m_retired_swapchains;

public:
    Swapchain(
        VkPhysicalDevice pdev, VkDevice dev, VkSurfaceKHR surf,
        SizeCallback&& size_cb,
        const Image::Usage& img_usg, PresentMode pmode
    );

    const auto&         GetDescription() const { return m_description; } 

    ImageRef            AcquireImage(SemaphoreRef sem);
    void                PresentImage(VkQueue q, Image& img, SemaphoreRef sem);

private:
    RetiredSwapchain    create();
    void                recreate();
};
}
