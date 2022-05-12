#include "VKSwapchain.hpp"

#include <algorithm>
#include <utility>

namespace R1::VK {
VkSurfaceCapabilitiesKHR getSurfaceCapabilities(
    VkPhysicalDevice dev, VkSurfaceKHR surf
) {
    VkSurfaceCapabilitiesKHR caps;
    if (vkGetPhysicalDeviceSurfaceCapabilitiesKHR(dev, surf, &caps)) {
        throw std::runtime_error{"Vulkan: failed to retrieve surface capabilities"};
    }
    return caps;
}

size_t getSurfaceFormatCount(VkPhysicalDevice dev, VkSurfaceKHR surf) {
    uint32_t surf_fmt_cnt = 0;
    vkGetPhysicalDeviceSurfaceFormatsKHR(dev, surf, &surf_fmt_cnt, nullptr);
    return surf_fmt_cnt;
}

size_t getSurfacePresentModeCount(VkPhysicalDevice dev, VkSurfaceKHR surf) {
    uint32_t pmode_cnt = 0;
    vkGetPhysicalDeviceSurfacePresentModesKHR(
        dev, surf, &pmode_cnt, nullptr
    );
    return pmode_cnt;
}

namespace {
size_t getSwapchainImageCount(VkDevice dev, VkSwapchainKHR swc) {
    uint32_t img_cnt = 0;
    vkGetSwapchainImagesKHR(dev, swc, &img_cnt, nullptr);
    return img_cnt;
}

std::vector<VkImage> getSwapchainImages(VkDevice dev, VkSwapchainKHR swc) {
    uint32_t img_cnt = getSwapchainImageCount(dev, swc);
    std::vector<VkImage> imgs(img_cnt);
    auto r = vkGetSwapchainImagesKHR(dev, swc, &img_cnt, imgs.data());
    if (r) {
        return {};
    }
    return imgs;
}

unsigned selectSwapchainImageCount(const VkSurfaceCapabilitiesKHR& caps, unsigned img_cnt) {
    img_cnt = std::max(img_cnt, caps.minImageCount);
    if (caps.maxImageCount) {
        img_cnt = std::min(img_cnt, caps.maxImageCount);
    }
    return img_cnt;
}

template<std::ranges::range R>
VkSurfaceFormatKHR selectSwapchainFormat(
    R&& r, VkPhysicalDevice dev, VkSurfaceKHR surf
) {
    static constexpr std::array srb_fmts = {
        VK_FORMAT_R8G8B8_SRGB,
        VK_FORMAT_B8G8R8_SRGB,
        VK_FORMAT_R8G8B8A8_SRGB,
        VK_FORMAT_B8G8R8A8_SRGB,
    };

    constexpr VkSurfaceFormatKHR error_fmt = {
        .format = VK_FORMAT_UNDEFINED
    };

    if (std::empty(r)) {
        return error_fmt;
    }

    auto it = std::ranges::find_first_of(
        r, srb_fmts, {},
        [] (const auto& sf) { return sf.format; }
    );
    if (it != r.end()) {
        return *it;
    }

    return *r.begin();
}

bool operator==(const VkExtent2D& l, const VkExtent2D& r) {
    return l.width == r.width and l.height == r.height;
}

VkExtent2D selectSwapchainExtent(const VkSurfaceCapabilitiesKHR& caps, VkExtent2D ext) {
    constexpr VkExtent2D spec_val = { 0xFFFFFFFF, 0xFFFFFFFF };
    auto cur_ext = caps.currentExtent;
    if (cur_ext == spec_val) {
        auto min_ext = caps.minImageExtent;
        auto max_ext = caps.maxImageExtent;
        ext.width = std::clamp(ext.width, min_ext.width, max_ext.width);
        ext.height = std::clamp(ext.height, min_ext.height, max_ext.height);
        return ext;
    }
    else {
        return cur_ext;
    }
}

bool swapchainImageUsageFlagsSupported(const VkSurfaceCapabilitiesKHR& caps, VkImageUsageFlags usg_flgs) {
    return usg_flgs == (caps.supportedUsageFlags & usg_flgs);
}

VkCompositeAlphaFlagBitsKHR selectSwapchainCompositeAlpha(const VkSurfaceCapabilitiesKHR& caps) {
    static constexpr std::array composite_alphas = {
        VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
        VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR,
        VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR,
        VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR,
    };

    return *std::ranges::find_if(
        composite_alphas, [&] (const auto& ca) { return ca & caps.supportedCompositeAlpha; }
    );
}

template<std::ranges::range R>
bool swapchainPresentModeSupported(R&& r, VkPresentModeKHR pmode) {
    return std::ranges::find(r, pmode) != r.end();
}

VkSwapchainKHR createSwapchain(
    VkDevice dev, VkSurfaceKHR surf,
    const SwapchainDescription& desc,
    VkSwapchainKHR old_swapchain
) {
    VkSwapchainCreateInfoKHR create_info = {
        .sType = sType(create_info),
        .surface = surf,
        .minImageCount = desc.image_count,
        .imageFormat = desc.surface_format.format,
        .imageColorSpace = desc.surface_format.colorSpace,
        .imageExtent = { desc.width, desc.height },
        .imageArrayLayers = 1,
        .imageUsage = desc.image_usage_flags,
        .imageSharingMode = VK_SHARING_MODE_EXCLUSIVE,
        .preTransform = desc.surface_pre_transform,
        .compositeAlpha = desc.composite_alpha,
        .presentMode = desc.present_mode,
        .clipped = true,
        .oldSwapchain = old_swapchain,
    };
    
    VkSwapchainKHR swc = VK_NULL_HANDLE;
    vkCreateSwapchainKHR(dev, &create_info, nullptr, &swc);
    return swc;
}
}

Swapchain::Swapchain(
    VkPhysicalDevice pdev, VkDevice dev,
    VkSurfaceKHR surf, SizeCallback&& size_cb,
    VkImageUsageFlags img_usg_flgs, VkPresentModeKHR pmode
):  m_physical_device{pdev},
    m_surface{surf},
    m_swapchain{dev, VK_NULL_HANDLE},
    m_description {
        .image_usage_flags = img_usg_flgs,
        .present_mode = pmode,
    },
    m_size_callback{std::move(size_cb)}
{
    create();
}

Swapchain::RetiredSwapchain Swapchain::create() {
    m_capabilities = getSurfaceCapabilities(m_physical_device, m_surface);
    m_formats.resize(getSurfaceFormatCount(m_physical_device, m_surface));
    getSurfaceFormats(m_physical_device, m_surface, m_formats.begin());
    m_present_modes.resize(getSurfacePresentModeCount(m_physical_device, m_surface));
    getSurfacePresentModes(m_physical_device, m_surface, m_present_modes.begin());

    constexpr auto c_pref_img_cnt = 2;
    m_description.image_count = selectSwapchainImageCount(m_capabilities, c_pref_img_cnt);
    m_description.surface_format = selectSwapchainFormat(m_formats, m_physical_device, m_surface);
    std::tie(m_description.width, m_description.height) = [&] {
        // On some platforms, currentExtent becomes (0, 0) when a window is minimized.
        // Wait until it is shown again to create a swapchain.
        VkExtent2D ext;
        do {
            std::tie(ext.width, ext.height) = m_size_callback();
            ext = selectSwapchainExtent(m_capabilities, ext);
        } while (ext == VkExtent2D{0, 0});
        return std::tuple<unsigned, unsigned>{ext.width, ext.height};
    } ();
    assert(swapchainImageUsageFlagsSupported(m_capabilities, m_description.image_usage_flags));
    m_description.surface_pre_transform = m_capabilities.currentTransform;
    m_description.composite_alpha = selectSwapchainCompositeAlpha(m_capabilities);
    assert(swapchainPresentModeSupported(m_present_modes, m_description.present_mode));

    auto dev = m_swapchain.get_device();

    Vk::Swapchain new_swapchain {
        dev, createSwapchain(dev, m_surface, m_description, m_swapchain.get())
    };
    auto new_images = getSwapchainImages(dev, new_swapchain.get());
    m_description.image_count = new_images.size();

    return {
        .swapchain = std::exchange(m_swapchain, std::move(new_swapchain)),
        .images = std::exchange(m_images, std::move(new_images)),
        .acquired_count = std::exchange(m_acquired_count, 0),
    };
}

void Swapchain::recreate() {
    m_retired_swapchains.emplace_back(create());
}

Swapchain::AcquireInfo Swapchain::acquireImage(VkSemaphore signal_sem) {
    VkImage img;
    unsigned width, height;
    do {
        uint32_t idx;
        auto r = vkAcquireNextImageKHR(
            m_swapchain.get_device(), m_swapchain.get(),
            UINT64_MAX, signal_sem, VK_NULL_HANDLE, &idx
        );
        if (r == VK_ERROR_OUT_OF_DATE_KHR) {
            recreate();
        } else if (r != VK_SUCCESS and r != VK_SUBOPTIMAL_KHR) {
            throw std::runtime_error{"Vulkan: Failed to acquire swapchain image"};
        } else {
            img = m_images[idx];
            width = m_description.width;
            height = m_description.height;
            m_acquired_count++;
            if (r == VK_SUBOPTIMAL_KHR) {
                recreate();
            }
            break;
        }
    } while (true);
    return {img, width, height};
};

void Swapchain::present(VkQueue q, VkImage img, std::span<VkSemaphore> sems) {
    auto get_img_idx = [] (const std::vector<VkImage>& imgs, VkImage img) {
        return std::ranges::find(imgs, img) - imgs.begin();
    };
    auto present = [&] (VkSwapchainKHR swc, uint32_t idx) {
        VkPresentInfoKHR present_info = {
            .sType = sType(present_info),
            .waitSemaphoreCount =
                static_cast<uint32_t>(sems.size()),
            .pWaitSemaphores = sems.data(),
            .swapchainCount = 1,
            .pSwapchains = &swc,
            .pImageIndices = &idx,
        };
        auto r = vkQueuePresentKHR(q, &present_info);
        if (r != VK_SUCCESS and r != VK_SUBOPTIMAL_KHR and r != VK_ERROR_OUT_OF_DATE_KHR) {
            throw std::runtime_error{"Vulkan: Failed to present swapchain image"};
        }
    };

    uint32_t idx = get_img_idx(m_images, img);
    bool is_retired = idx == m_images.size();
    if (!is_retired) {
        auto swc = m_swapchain.get();
        present(swc, idx);
        m_acquired_count--;
    } else {
        auto it = m_retired_swapchains.begin();
        for (;; ++it) {
            assert(it != m_retired_swapchains.end());
            idx = get_img_idx(it->images, img);
            if (idx < it->images.size()) {
                break;
            }
        }
        auto swc = it->swapchain.get();
        present(swc, idx);
        it->acquired_count--;
        if (!it->acquired_count) {
            m_retired_swapchains.erase(it);
        }
    }
}
}
