#include "ContextImpl.hpp"
#include "InstanceImpl.hpp"
#include "SwapchainImpl.hpp"
#include "VKUtil.hpp"

#include <ranges>

namespace R1::VK {
void DestroySurface(Instance instance, Surface surface) {
    vkDestroySurfaceKHR(instance->instance.get(), surface, nullptr);
}

namespace {
Vk::Swapchain CreateSwapchainHandle(
    VkDevice dev,
    const SwapchainDescription& desc,
    VkSwapchainKHR old_swapchain
) {
    VkSwapchainCreateInfoKHR create_info = {
        .sType = sType(create_info),
        .flags = SwapchainCapabilitiesToVK(desc.capabilities),
        .surface = desc.surface,
        .minImageCount = desc.image_count,
        .imageFormat = static_cast<VkFormat>(desc.format),
        .imageColorSpace = static_cast<VkColorSpaceKHR>(desc.color_space),
        .imageExtent = {
            .width = desc.width,
            .height = desc.height,
        },
        .imageArrayLayers = 1,
        .imageUsage = ImageUsageToVK(desc.image_usage),
        .imageSharingMode = desc.image_sharing_queue_families.empty() ?
            VK_SHARING_MODE_EXCLUSIVE: VK_SHARING_MODE_CONCURRENT,
        .queueFamilyIndexCount =
            static_cast<uint32_t>(desc.image_sharing_queue_families.size()),
        .pQueueFamilyIndices = reinterpret_cast<const unsigned*>(
            desc.image_sharing_queue_families.data()),
        .preTransform =
            static_cast<VkSurfaceTransformFlagBitsKHR>(desc.transform),
        .compositeAlpha =
            static_cast<VkCompositeAlphaFlagBitsKHR>(desc.composite_alpha),
        .presentMode =
            static_cast<VkPresentModeKHR>(desc.present_mode),
        .clipped = desc.capabilities.clipped,
        .oldSwapchain = old_swapchain,
    };

    VkSwapchainKHR swc = VK_NULL_HANDLE;
    vkCreateSwapchainKHR(dev, &create_info, nullptr, &swc);

    return Vk::Swapchain{dev, swc};
}

void InitSwapchain(
    Swapchain swc,
    VkSwapchainKHR old_swapchain = VK_NULL_HANDLE
) {
    auto dev = swc->handle.get_device();
    auto [width, height] = swc->surface_size_cb();
    swc->description.width = width;
    swc->description.height = height;
    swc->handle = CreateSwapchainHandle(
        dev, swc->description, old_swapchain);
    if (!swc->handle) {
        throw std::runtime_error{"Vulkan: Failed to create swapchain"};
    }

    auto images = Enumerate<VkImage>(
        dev, swc->handle.get(), vkGetSwapchainImagesKHR);
    auto v = std::views::transform(images,
        [&](VkImage image) {
            return ImageImpl {
                .image{dev, image},
                .allocation{nullptr, nullptr},
            };
        });
    swc->images.assign(v.begin(), v.end());
    swc->description.image_count = swc->images.size();
}

void UpdateSwapchain(Swapchain swc) {
    auto old_handle = std::move(swc->handle);
    InitSwapchain(swc, old_handle.get());
    // Before destroying old swapchain
    QueueWaitIdle(swc->present_queue);
}
}

Swapchain CreateSwapchain(
    Context ctx,
    Surface surface,
    SurfaceSizeCallback size_cb,
    const SwapchainConfig& config
) {
    auto dev = ctx->device.get();
    auto swc = std::make_unique<SwapchainImpl>(SwapchainImpl{
        .handle{dev, nullptr},
        .present_queue = config.present_queue,
        .surface_size_cb = std::move(size_cb),
        .description = {
            .surface = surface,
            .capabilities = config.capabilities,
            .image_usage = config.image_usage,
            .format = config.format,
            .color_space = config.color_space,
            .image_count = config.image_count,
            .image_sharing_queue_families {
                config.image_sharing_queue_families.begin(),
                config.image_sharing_queue_families.end()
            },
            .transform = config.transform,
            .composite_alpha = config.composite_alpha,
            .present_mode = config.present_mode,
        },
    });
    InitSwapchain(swc.get());
    return swc.release();
}

void DestroySwapchain(Swapchain swapchain) {
    QueueWaitIdle(swapchain->present_queue);
    delete swapchain;
}

const SwapchainDescription& GetSwapchainDescription(
    Swapchain swapchain
) {
    return swapchain->description;
}

unsigned GetSwapchainImageCount(Swapchain swapchain) {
    return GetSwapchainDescription(swapchain).image_count;
}

Image GetSwapchainImage(Swapchain swapchain, unsigned image_idx) {
    return &swapchain->images[image_idx];
}

unsigned AcquireImage(
    Swapchain swapchain,
    std::chrono::nanoseconds timeout,
    Semaphore signal_semaphore,
    Fence signal_fence
) {
    while (true) {
        uint32_t image_idx = 0;
        auto r = vkAcquireNextImageKHR(
            swapchain->handle.get_device(), swapchain->handle.get(),
            timeout.count(),
            signal_semaphore, signal_fence,
            &image_idx); 
        if (r == VK_SUCCESS or r == VK_SUBOPTIMAL_KHR) {
            return image_idx;
        } else if (r == VK_ERROR_OUT_OF_DATE_KHR) {
            UpdateSwapchain(swapchain);
        } else {
            throw std::runtime_error{
                "Vulkan: Failed to acquire image from swapchain"};
        }
    }
}

void PresentImage(
    Swapchain swapchain,
    unsigned image_idx,
    std::span<const Semaphore> wait_semaphores
) {
    auto vk_swc = swapchain->handle.get();
    VkPresentInfoKHR present_info = {
        .sType = sType(present_info),
        .waitSemaphoreCount =
            static_cast<uint32_t>(wait_semaphores.size()),
        .pWaitSemaphores = wait_semaphores.data(),
        .swapchainCount = 1,
        .pSwapchains = &vk_swc,
        .pImageIndices = &image_idx,
    };
    auto r = vkQueuePresentKHR(swapchain->present_queue, &present_info);
    if (r == VK_SUBOPTIMAL_KHR or r == VK_ERROR_OUT_OF_DATE_KHR) {
        UpdateSwapchain(swapchain);
    } else if (r) {
        throw std::runtime_error{
            "Vulkan: Failed to present to swapchain"};
    }
}
}
