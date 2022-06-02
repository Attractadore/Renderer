#include "VKSwapchain.hpp"
#include "VKTypes.hpp"

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
auto getSurfaceFormats(
    VkPhysicalDevice dev, VkSurfaceKHR surf
) {
    std::vector<VkSurfaceFormatKHR> fmts(getSurfaceFormatCount(dev, surf));
    VK::getSurfaceFormats(dev, surf, fmts.begin());
    return fmts;
}

auto getSurfacePresentModes(
    VkPhysicalDevice dev, VkSurfaceKHR surf
) {
    std::vector<VkPresentModeKHR> pmodes(getSurfacePresentModeCount(dev, surf));
    VK::getSurfacePresentModes(dev, surf, pmodes.begin());
    return pmodes;
}

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
    constexpr VkSurfaceFormatKHR error_fmt = {
        .format = VK_FORMAT_UNDEFINED
    };

    if (std::empty(r)) {
        return error_fmt;
    }

    auto it = std::ranges::find_if(r,
        [] (const auto& fmt) {
            return IsSRGBFormat(static_cast<ImageFormat>(fmt));
        },
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

bool swapchainImageUsageSupported(const VkSurfaceCapabilitiesKHR& caps, const Image::Usage& usg) {
    VkImageUsageFlags usg_flgs = ImageUsageToFlags(usg);
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
bool swapchainPresentModeSupported(R&& r, PresentMode pmode) {
    return std::ranges::find(r, PresentModeToVK(pmode)) != r.end();
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
    const Image::Usage& img_usg, PresentMode pmode
):  m_physical_device{pdev},
    m_surface{surf},
    m_swapchain{dev, VK_NULL_HANDLE},
    m_description {
        .image_description = {
            .usage = img_usg,
        },
        .present_mode = pmode,
    },
    m_size_callback{std::move(size_cb)}
{
    create();
}

Swapchain::RetiredSwapchain Swapchain::create() {
    auto surface_capabilities = getSurfaceCapabilities(m_physical_device, m_surface);
    auto surface_formats = getSurfaceFormats(m_physical_device, m_surface);
    auto surface_present_modes = getSurfacePresentModes(m_physical_device, m_surface);

    constexpr auto c_pref_img_cnt = 2;
    auto image_count = selectSwapchainImageCount(surface_capabilities, c_pref_img_cnt);
    auto surface_format = selectSwapchainFormat(surface_formats, m_physical_device, m_surface);
    auto [width, height] = [&] {
        // On some platforms, currentExtent becomes (0, 0) when a window is minimized.
        // Wait until it is shown again to create a swapchain.
        VkExtent2D ext;
        do {
            std::tie(ext.width, ext.height) = m_size_callback();
            ext = selectSwapchainExtent(surface_capabilities, ext);
        } while (ext == VkExtent2D{0, 0});
        return std::tuple<unsigned, unsigned>{ext.width, ext.height};
    } ();
    auto usg = m_description.image_description.usage;
    assert(swapchainImageUsageSupported(surface_capabilities, usg));
    auto surface_pre_transform = surface_capabilities.currentTransform;
    auto composite_alpha = selectSwapchainCompositeAlpha(surface_capabilities);
    auto present_mode = m_description.present_mode;
    assert(swapchainPresentModeSupported(surface_present_modes, present_mode));

    auto dev = m_swapchain.get_device();

    SwapchainDescription description = {
        .width = width,
        .height = height,
        .image_count = image_count,
        .image_usage_flags = ImageUsageToFlags(usg),
        .surface_format = surface_format,
        .surface_pre_transform = surface_pre_transform,
        .composite_alpha = composite_alpha,
        .present_mode = PresentModeToVK(present_mode),
    };
    Vk::Swapchain new_swapchain {
        dev, createSwapchain(dev, m_surface, description, m_swapchain.get())
    };
    auto image_handles = getSwapchainImages(dev, new_swapchain.get());

    m_description = {
        .image_description = {
            .type = Image::Type::D2,
            .format = static_cast<ImageFormat>(surface_format.format),
            .width = width,
            .height = height,
            .depth = 1,
            .mip_level_count = 1,
            .array_layer_count = 1,
            .sample_count = 1,
            .tiling = Image::Tiling::Optimal,
            .usage = usg,
        },
        .image_count = static_cast<unsigned>(image_handles.size()),
        .present_mode = present_mode,
    };

    std::vector<SwapchainImageRef> new_images(image_handles.size());
    std::ranges::transform(image_handles, new_images.begin(),
        [&] (VkImage img) {
            return Image::Create(Vk::Image{dev, img}, m_description.image_description);
        }
    );

    return {
        .swapchain = std::exchange(m_swapchain, std::move(new_swapchain)),
        .images = std::exchange(m_images, std::move(new_images)),
        .signal_semaphores = std::exchange(
            m_signal_semaphores, std::vector<SemaphoreRef>(m_description.image_count)
        ),
        .present_semaphores = std::exchange(
            m_present_semaphores, std::vector<SemaphoreRef>(m_description.image_count)
        ),
        .acquired_count = std::exchange(m_acquired_count, 0),
    };
}

void Swapchain::recreate() {
    m_retired_swapchains.emplace_back(create());
}

ImageRef Swapchain::AcquireImage(SemaphoreRef signal_sem) {
    do {
        auto sem = signal_sem->m_semaphore.get();
        uint32_t idx;
        auto r = vkAcquireNextImageKHR(
            m_swapchain.get_device(), m_swapchain.get(),
            UINT64_MAX, sem, VK_NULL_HANDLE, &idx
        );
        if (r == VK_SUCCESS or r == VK_SUBOPTIMAL_KHR) {
            ImageRef img = m_images[idx];
            // TODO: the previous semaphore used to acquire an image
            // can still be in use when the image is acquired again
            m_signal_semaphores[idx] = std::move(signal_sem);
            m_acquired_count++;
            if (r == VK_SUBOPTIMAL_KHR) {
                recreate();
            }
            return img;
        } else if (r == VK_ERROR_OUT_OF_DATE_KHR) {
            recreate();
        } else {
            throw std::runtime_error{"Vulkan: Failed to acquire swapchain image"};
        }
    } while (true);
};

void Swapchain::PresentImage(VkQueue q, Image& img, SemaphoreRef present_sem) {
    auto get_img_idx = [] (const std::vector<SwapchainImageRef>& imgs, Image& img) {
        return std::ranges::find_if(imgs, [&] (const auto& swc_img_ref) {
            return swc_img_ref.get() == &img;
        }) - imgs.begin();
    };

    auto present = [&] (VkSwapchainKHR swc, uint32_t idx) {
        auto vksem = present_sem->m_semaphore.get();
        VkPresentInfoKHR present_info = {
            .sType = sType(present_info),
            .waitSemaphoreCount = 1,
            .pWaitSemaphores = &vksem,
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
        // TODO: the previous semaphore used to present an image
        // can still be in use when the image is presented again,
        // and it is impossible to tell whether a previous present
        // has finished (except by waiting on the semaphore signaled
        // by acquire?)
        m_present_semaphores[idx] = std::move(present_sem);
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
        it->present_semaphores[idx] = std::move(present_sem);
        it->acquired_count--;
        if (!it->acquired_count) {
            // TODO: swapchain destruction mustn't happen before
            // all (present) operations on its images are completed
            m_retired_swapchains.erase(it);
        }
    }
}
}
