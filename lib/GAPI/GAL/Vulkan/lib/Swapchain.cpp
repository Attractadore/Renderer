#include "Common/Vector.hpp"
#include "ContextImpl.hpp"
#include "SwapchainImpl.hpp"
#include "VKUtil.hpp"
#include "InstanceImpl.hpp"

#include <utility>

namespace R1::GAL::Vulkan {
namespace {
VkSurfaceCapabilitiesKHR GetVkSurfaceCapabilities(
    VkPhysicalDevice device, VkSurfaceKHR surface
) {
    VkSurfaceCapabilitiesKHR caps;
    ThrowIfFailed(
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &caps),
        "Vulkan: Failed to get surface capabilities");
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
    return {
        .supported_formats = std::move(formats),
        .supported_present_modes = std::move(present_modes),
        .supported_composite_alpha = caps.supportedCompositeAlpha,
        .min_image_count = caps.minImageCount,
        .max_image_count = caps.maxImageCount,
        .supported_image_usage = caps.supportedUsageFlags,
    };
}
}

SurfaceDescription GetSurfaceDescription(VkSurfaceKHR surface, Device device) {
    return FillSurfaceDescription(device->physical_device, surface);
}

SwapchainImpl::SwapchainImpl(
    Context ctx,
    VkSurfaceKHR surf,
    SurfaceSizeCallback size_cb,
    const SwapchainConfig& config
):  ctx{ctx},
    surface_size_cb{std::move(size_cb)},
    create_info {
        .sType = SType(create_info),
        .surface = surf,
        .minImageCount = config.image_count,
        .imageFormat = config.format,
        .imageColorSpace = config.color_space,
        .imageArrayLayers = 1,
        .imageUsage = config.image_usage,
        .imageSharingMode = VK_SHARING_MODE_EXCLUSIVE,
        .compositeAlpha = config.composite_alpha,
        .presentMode = config.present_mode,
        .clipped = config.clipped,
    } {}

Swapchain CreateSwapchain(
    Context ctx,
    VkSurfaceKHR surface,
    SurfaceSizeCallback size_cb,
    const SwapchainConfig& config
) {
    auto swc = std::make_unique<SwapchainImpl>(
        ctx, surface, std::move(size_cb), config);
    swc->Init();
    return swc.release();
}

void SwapchainImpl::Init(
    VkSwapchainKHR old_swapchain
) {
    auto caps = GetVkSurfaceCapabilities(ctx->adapter, create_info.surface);

    create_info.imageExtent = [&] {
        constexpr auto special_value = 0xFFFFFFFF;
        if (caps.currentExtent.width == special_value and
            caps.currentExtent.height == special_value) {
            auto [width, height] = surface_size_cb();
            return VkExtent2D{width, height};
        } else {
            return caps.currentExtent;
        }
    } ();
    create_info.preTransform = caps.currentTransform;
    create_info.oldSwapchain = old_swapchain;

    ThrowIfFailed(
        ctx->CreateSwapchainKHR(&create_info, &swapchain),
        "Vulkan: Failed to create swapchain");

    auto swc_images = Enumerate<VkImage>(
        ctx->GetDevice(), swapchain, ctx->vk.GetSwapchainImagesKHR);
    auto v = std::views::transform(swc_images,
        [] (VkImage image) {
            return ImageImpl {
                .image = image,
            };
        });

    images.assign(v.begin(), v.end());
}

void SwapchainImpl::Clear() {
    ctx->DestroySwapchainKHR(swapchain);
    images.clear();
}

SwapchainImpl::~SwapchainImpl() {
    Clear();
}

void DestroySwapchain(Swapchain swapchain) {
    delete swapchain;
}

std::tuple<unsigned, unsigned> SwapchainImpl::Size() const noexcept {
    auto ext = create_info.imageExtent;
    return { ext.width, ext.height };
}

std::tuple<unsigned, unsigned> GetSwapchainSize(Swapchain swapchain) {
    return swapchain->Size();
}

unsigned GetSwapchainImageCount(Swapchain swapchain) {
    return swapchain->ImageCount();
}

Image GetSwapchainImage(Swapchain swapchain, size_t image_idx) {
    return swapchain->GetImage(image_idx);
}

std::tuple<unsigned, SwapchainStatus> SwapchainImpl::AcquireImage(
    VkSemaphore signal_semaphore
) {
    uint32_t image_idx = 0;
    auto r = ctx->AcquireNextImageKHR(swapchain,
        UINT64_MAX, signal_semaphore, nullptr, &image_idx); 
    switch (r) {
        case VK_SUCCESS:
        case VK_SUBOPTIMAL_KHR:
            return {image_idx, SwapchainStatus::Optimal};
        case VK_ERROR_OUT_OF_DATE_KHR:
            return {-1, SwapchainStatus::OutOfDate};
        default:
            throw std::runtime_error{
                "Vulkan: Failed to acquire image from swapchain"};
    }
}

std::tuple<unsigned, SwapchainStatus> AcquireImage(
    Swapchain swapchain,
    VkSemaphore signal_semaphore
) {
    return swapchain->AcquireImage(signal_semaphore);
}

SwapchainStatus SwapchainImpl::PresentImage(
    VkQueue queue,
    unsigned image_idx,
    VkSemaphore wait_semaphore
) {
    VkPresentInfoKHR present_info = {
        .sType = SType(present_info),
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = &wait_semaphore,
        .swapchainCount = 1,
        .pSwapchains = &swapchain,
        .pImageIndices = &image_idx,
    };
    auto r = ctx->QueuePresentKHR(queue, &present_info);
    if (!r) {
        auto [w, h] = surface_size_cb();
        const auto& ext = create_info.imageExtent;
        if (ext.width != w or ext.height != h) {
            r = VK_ERROR_OUT_OF_DATE_KHR;
        }
    }
    switch (r) {
        case VK_SUCCESS:
            return SwapchainStatus::Optimal;
        case VK_SUBOPTIMAL_KHR:
        case VK_ERROR_OUT_OF_DATE_KHR:
            return SwapchainStatus::OutOfDate;
        default:
            throw std::runtime_error{
                "Vulkan: Failed to present to swapchain"};
    }
}

SwapchainStatus PresentImage(
    VkQueue queue,
    Swapchain swapchain,
    unsigned image_idx,
    VkSemaphore wait_semaphore
) {
    return swapchain->PresentImage(queue, image_idx, wait_semaphore);
}

void SwapchainImpl::Resize() {
    Handle old_handle {
        std::exchange(swapchain, VK_NULL_HANDLE),
        [&] (VkSwapchainKHR swc) { ctx->DestroySwapchainKHR(swc); }};
    Clear();
    Init(old_handle.get());
}

void ResizeSwapchain(Swapchain swapchain) {
    swapchain->Resize();
}
}
