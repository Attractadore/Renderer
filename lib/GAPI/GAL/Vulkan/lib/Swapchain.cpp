#include "Common/Vector.hpp"
#include "ContextImpl.hpp"
#include "DeviceImpl.hpp"
#include "GAL/Swapchain.hpp"
#include "ImageImpl.hpp"
#include "InstanceImpl.hpp"
#include "SwapchainImpl.hpp"
#include "VKUtil.hpp"

namespace R1::GAL {
namespace {
VkSurfaceCapabilitiesKHR GetVkSurfaceCapabilities(
    VkPhysicalDevice device, VkSurfaceKHR surface
) {
    VkSurfaceCapabilitiesKHR caps;
    auto r = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
        device, surface, &caps); 
    if (r) {
        throw std::runtime_error{
            "Vulkan: Failed to get surface capabilities"};
    }
    return caps;
}

SurfaceDescription FillSurfaceDescription(
    VkPhysicalDevice device, VkSurfaceKHR surface
) {
    auto caps = GetVkSurfaceCapabilities(device, surface);
    auto formats = Enumerate<VkSurfaceFormatKHR>(
        device, surface, vkGetPhysicalDeviceSurfaceFormatsKHR);
    auto present_modes = Enumerate<VkPresentModeKHR>(
        device, surface, vkGetPhysicalDeviceSurfacePresentModesKHR);

    auto format_v = std::views::transform(formats,
        [] (VkSurfaceFormatKHR sf) {
            return SurfaceFormat {
                .format = static_cast<Format>(sf.format),
                .color_space = static_cast<ColorSpace>(sf.colorSpace),
            };
        });
    auto present_mode_v = std::views::transform(present_modes,
        [] (VkPresentModeKHR pm) { return static_cast<PresentMode>(pm); });

    return {
        .supported_formats = vec_from_range(format_v),
        .supported_present_modes = vec_from_range(present_mode_v),
        .supported_composite_alphas =
            ExtractEnum<CompositeAlpha>(caps.supportedCompositeAlpha),
        .min_image_count = caps.minImageCount,
        .max_image_count = caps.maxImageCount,
        .supported_image_usage =
            static_cast<ImageUsage>(caps.supportedUsageFlags),
    };
}

auto FillSurfaceDescriptions(
    Instance instance, VkSurfaceKHR surface
) {
    auto v = std::views::transform(instance->devices,
        [&] (DeviceImpl& device) {
            return std::pair{
                &device,
                FillSurfaceDescription(device.physical_device, surface)};
        });
    return std::unordered_map{v.begin(), v.end()};
}
}

Surface CreateSurfaceFromHandle(Instance instance, Vk::Surface handle) {
    auto surf = handle.get();
    return std::make_unique<SurfaceImpl>(SurfaceImpl{
        .handle = std::move(handle),
        .descriptions = FillSurfaceDescriptions(instance, surf),
    }).release();
}

void DestroySurface(Surface surface) {
    delete surface;
}

const SurfaceDescription& GetSurfaceDescription(Surface surface, Device device) {
    return surface->descriptions.find(device)->second;
}

namespace {
void InitSwapchain(
    Swapchain swc
) {
    auto dev = swc->handle.get_device();
    auto old_swapchain = swc->handle.get();
    auto caps = GetVkSurfaceCapabilities(swc->adapter, swc->create_info.surface);

    swc->create_info.sType = sType(swc->create_info);
    swc->create_info.imageExtent = [&] {
        constexpr auto special_value = 0xFFFFFFFF;
        if (caps.currentExtent.width == special_value and
            caps.currentExtent.height == special_value) {
            auto [width, height] = swc->surface_size_cb();
            return VkExtent2D{width, height};
        } else {
            return caps.currentExtent;
        }
    } ();
    swc->create_info.preTransform = caps.currentTransform;
    swc->create_info.oldSwapchain = old_swapchain;

    swc->handle.reset(
    [] (VkDevice device, const VkSwapchainCreateInfoKHR& create_info) {
        VkSwapchainKHR swc = VK_NULL_HANDLE;
        vkCreateSwapchainKHR(device, &create_info, nullptr, &swc);
        return swc;
    } (dev, swc->create_info));

    if (!swc->handle) {
        throw std::runtime_error{"Vulkan: Failed to create swapchain"};
    }

    auto images = Enumerate<VkImage>(
        dev, swc->handle.get(), vkGetSwapchainImagesKHR);
    auto v = std::views::transform(images,
        [&](VkImage image) {
            return SwapchainImageImpl{ImageImpl{
                .image{dev, image},
                .allocation{nullptr, nullptr},
            }};
        });
    swc->images.assign(v.begin(), v.end());
}
}

Swapchain CreateSwapchain(
    Context ctx,
    Surface surface,
    SurfaceSizeCallback size_cb,
    const SwapchainConfig& config
) {
    auto dev = ctx->device.get();
    auto pdev = ctx->adapter;
    auto surf = surface->handle.get();
    auto swc = std::make_unique<SwapchainImpl>(SwapchainImpl{
        .adapter = pdev,
        .handle{dev, nullptr},
        .present_queue = config.present_queue,
        .surface_size_cb = std::move(size_cb),
        .create_info = {
            .surface = surf,
            .minImageCount = config.image_count,
            .imageFormat = static_cast<VkFormat>(config.format),
            .imageColorSpace = static_cast<VkColorSpaceKHR>(config.color_space),
            .imageArrayLayers = 1,
            .imageUsage = static_cast<VkImageUsageFlags>(config.image_usage.Extract()),
            .imageSharingMode = config.image_sharing_queue_families.empty() ?
                VK_SHARING_MODE_EXCLUSIVE: VK_SHARING_MODE_CONCURRENT,
            .queueFamilyIndexCount =
                static_cast<uint32_t>(config.image_sharing_queue_families.size()),
            .pQueueFamilyIndices = reinterpret_cast<const unsigned*>(
                config.image_sharing_queue_families.data()),
            .compositeAlpha =
                static_cast<VkCompositeAlphaFlagBitsKHR>(config.composite_alpha),
            .presentMode =
                static_cast<VkPresentModeKHR>(config.present_mode),
            .clipped = config.clipped,
        },
    });
    InitSwapchain(swc.get());
    return swc.release();
}

void DestroySwapchain(Swapchain swapchain) {
    delete swapchain;
}

std::tuple<unsigned, unsigned> GetSwapchainSize(Swapchain swapchain) {
    auto ext = swapchain->create_info.imageExtent;
    return { ext.width, ext.height };
}

unsigned GetSwapchainImageCount(Swapchain swapchain) {
    return swapchain->images.size();
}

Image GetSwapchainImage(Swapchain swapchain, unsigned image_idx) {
    return &swapchain->images[image_idx];
}

std::tuple<unsigned, SwapchainStatus> AcquireImage(
    Swapchain swapchain,
    Semaphore signal_semaphore
) {
    uint32_t image_idx = 0;
    auto r = vkAcquireNextImageKHR(
        swapchain->handle.get_device(), swapchain->handle.get(),
        UINT64_MAX,
        signal_semaphore, nullptr,
        &image_idx); 
    switch (r) {
        case VK_SUCCESS:
        case VK_SUBOPTIMAL_KHR:
            return {image_idx, SwapchainStatus::Good};
        case VK_ERROR_OUT_OF_DATE_KHR:
            return {-1, SwapchainStatus::RequiresSlowResize};
    }
    throw std::runtime_error{
        "Vulkan: Failed to acquire image from swapchain"};
}

SwapchainStatus PresentImage(
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
    auto r = vkQueuePresentKHR(
        swapchain->present_queue, &present_info);
    switch (r) {
        case VK_SUCCESS:
            return SwapchainStatus::Good;
        case VK_SUBOPTIMAL_KHR:
        case VK_ERROR_OUT_OF_DATE_KHR:
            return SwapchainStatus::RequiresSlowResize;
    }
    throw std::runtime_error{
        "Vulkan: Failed to present to swapchain"};
}

void ResizeSwapchain(Swapchain swapchain) {
    InitSwapchain(swapchain);
}
}
