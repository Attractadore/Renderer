#include "Common/Vector.hpp"
#include "ContextImpl.hpp"
#include "SwapchainImpl.hpp"
#include "InstanceImpl.hpp"
#include "SyncImpl.hpp"
#include "VKUtil.hpp"

#include <utility>

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

VkResult WaitForFenceAndReset(VkDevice dev, VkFence fence) {
    auto r = vkWaitForFences(dev, 1, &fence, true, UINT64_MAX);
    if (r) {
        return r;
    }
    return vkResetFences(dev, 1, &fence);
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

SwapchainImpl::SwapchainImpl(
    VkPhysicalDevice pdev,
    VkDevice dev,
    VkSurfaceKHR surf,
    SurfaceSizeCallback size_cb,
    const SwapchainConfig& config
):  adapter{pdev},
    device{dev},
    present_queue{config.present_queue},
    surface_size_cb{std::move(size_cb)},
    create_info {
        .sType = SType(create_info),
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
    } {}

Swapchain CreateSwapchain(
    Context ctx,
    Surface surface,
    SurfaceSizeCallback size_cb,
    const SwapchainConfig& config
) {
    auto pdev = ctx->adapter;
    auto dev = ctx->device.get();
    auto surf = surface->handle.get();
    auto swc = std::make_unique<SwapchainImpl>(
        pdev, dev, surf, std::move(size_cb), config);
    swc->Init();
    return swc.release();
}

void SwapchainImpl::Init(
    VkSwapchainKHR old_swapchain
) {
    auto caps = GetVkSurfaceCapabilities(adapter, create_info.surface);

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

    ThrowIfFailed(vkCreateSwapchainKHR(
        device, &create_info, nullptr, &swapchain),
        "Vulkan: Failed to create swapchain");

    auto swapchain_images = Enumerate<VkImage>(
        device, swapchain, vkGetSwapchainImagesKHR);
    auto v = std::views::transform(swapchain_images,
        [&](VkImage image) {
            return SwapchainImpl::ImageData {
                .handle = {
                    .image = image,
                },
                .acquire_semaphore = CreateBinarySemaphore(device),
                // Fence 0 will be signaled by AcquireImage
                // Fences except fence 0 will be waited on by PresentImage 
                .acquire_fence = CreateFence(device, image != swapchain_images[0]),
                .present_semaphore = CreateBinarySemaphore(device),
                .present_fence = CreateFence(device, true),
            };
        });
    images.assign(v.begin(), v.end());
    acquire_idx = 0;
}

void SwapchainImpl::Clear() {
    vkDestroySwapchainKHR(
        device, swapchain, nullptr);
    std::ranges::for_each(images,
        [&] (SwapchainImpl::ImageData& image_data) {
            vkDestroySemaphore(
                device, image_data.acquire_semaphore, nullptr);
            vkDestroyFence(
                device, image_data.acquire_fence, nullptr);
            vkDestroySemaphore(
                device, image_data.present_semaphore, nullptr);
            vkDestroyFence(
                device, image_data.present_fence, nullptr);
        });
    images.clear();
}

SwapchainImpl::~SwapchainImpl() {
    Clear();
}

void DestroySwapchain(Swapchain swapchain) {
    delete swapchain;
}

std::tuple<unsigned, unsigned> SwapchainImpl::Size() const {
    auto ext = create_info.imageExtent;
    return { ext.width, ext.height };
}

std::tuple<unsigned, unsigned> GetSwapchainSize(Swapchain swapchain) {
    return swapchain->Size();
}

unsigned GetSwapchainImageCount(Swapchain swapchain) {
    return swapchain->ImageCount();
}

Image GetSwapchainImage(Swapchain swapchain, unsigned image_idx) {
    return swapchain->GetImage(image_idx);
}

std::tuple<unsigned, SwapchainStatus> SwapchainImpl::AcquireImage(
    const SemaphoreState* signal_state
) {
    // It's safe to use the current acquire semaphore here
    // because it has already been ensured that it is unsignaled
    // either in PresentImage, or in Init if this is a new swapchain.
    uint32_t image_idx = 0;
    auto r = vkAcquireNextImageKHR(device, swapchain,
        UINT64_MAX, GetCurrentAcquireSemaphore(), nullptr, &image_idx); 
    switch (r) {
        case VK_SUCCESS:
        case VK_SUBOPTIMAL_KHR:
            SignalAcquireSemaphore(signal_state);
            return {image_idx, SwapchainStatus::Optimal};
        case VK_ERROR_OUT_OF_DATE_KHR:
            return {-1, SwapchainStatus::OutOfDate};
    }
    throw std::runtime_error{
        "Vulkan: Failed to acquire image from swapchain"};
}

std::tuple<unsigned, SwapchainStatus> AcquireImage(
    Swapchain swapchain,
    const SemaphoreState* signal_state
) {
    return swapchain->AcquireImage(signal_state);
}

void SwapchainImpl::WaitOnAcquireFence() {
    // Wait for the previous acquire to complete before
    // reusing its semaphore.
    // This could be done in AcquireImage.
    // But AcquireImage reports that the swapchain is out of date,
    // it is likely that it will be called again.
    // In this case, the current acquire fence is already unsignaled,
    // and additional logic is required to prevent further waits on it.
    // But PresentImage is called only once, so no conditional
    // logic is required if hazard prevention is performed there instead.
    acquire_idx = (acquire_idx + 1) % images.size();
    ThrowIfFailed(WaitForFenceAndReset(device, GetCurrentAcquireFence()),
        "Vulkan: Failed to avoid acquire semaphore signal hazard");
}

void SwapchainImpl::SignalAcquireSemaphore(
    const SemaphoreState* signal_state
) {
    // Transform an internal binary semaphore into an
    // external timeline one and insert a fence to know
    // when the current acquire semaphore is safe to reuse.
    auto acq_sem = images[acquire_idx].acquire_semaphore;
    auto acq_fence = images[acquire_idx].acquire_fence;
    VkSemaphoreSubmitInfo wait_info = {
        .sType = SType(wait_info),
        .semaphore = acq_sem,
    };
    VkSemaphoreSubmitInfo signal_info = {
        .sType = SType(signal_info),
    };
    VkSubmitInfo2 submit_info = {
        .sType = SType(submit_info),
        .waitSemaphoreInfoCount = 1,
        .pWaitSemaphoreInfos = &wait_info,
    };
    if (signal_state) {
        signal_info.semaphore = signal_state->semaphore;
        signal_info.value = signal_state->value;
        submit_info.signalSemaphoreInfoCount = 1;
        submit_info.pSignalSemaphoreInfos = &signal_info;
    }
    ThrowIfFailed(vkQueueSubmit2(
        present_queue, 1, &submit_info, acq_fence),
        "Vulkan: Failed to signal semaphore");
}

SwapchainStatus SwapchainImpl::PresentImage(
    unsigned image_idx,
    std::span<const SemaphoreState> wait_states,
    const SemaphoreState* signal_state
) {
    WaitOnPresentFence(image_idx);
    MuxPresentSemaphores(image_idx, wait_states);
    auto status = QueuePresent(image_idx);
    SignalPresentSemaphore(image_idx, signal_state);
    WaitOnAcquireFence();
    return status;
}

SwapchainStatus PresentImage(
    Swapchain swapchain,
    unsigned image_idx,
    std::span<const SemaphoreState> wait_states,
    const SemaphoreState* signal_state
) {
    return swapchain->PresentImage(
        image_idx, wait_states, signal_state);
}

void SwapchainImpl::WaitOnPresentFence(unsigned image_idx) {
    // Wait for previous presentation of this image to complete
    // before reusing its semaphore.
    ThrowIfFailed(WaitForFenceAndReset(device, GetPresentFence(image_idx)),
        "Vulkan: Failed to avoid present semaphore wait hazard");
}

void SwapchainImpl::MuxPresentSemaphores(
    unsigned image_idx, std::span<const SemaphoreState> wait_states
) {
    // Mux external timeline semaphores into an internal binary one.
    static thread_local std::vector<VkSemaphoreSubmitInfo> wait_infos;
    auto v = std::views::transform(wait_states,
        [] (const SemaphoreState& state) {
            VkSemaphoreSubmitInfo info = {
                .sType = SType(info),
                .semaphore = state.semaphore,
                .value = state.value,
            };
            return info;
        });
    wait_infos.assign(v.begin(), v.end());
    VkSemaphoreSubmitInfo signal_info = {
        .sType = SType(signal_info),
        .semaphore = GetPresentSemaphore(image_idx),
    };
    VkSubmitInfo2 submit_info = {
        .sType = SType(submit_info),
        .waitSemaphoreInfoCount =
            static_cast<uint32_t>(wait_infos.size()),
        .pWaitSemaphoreInfos = wait_infos.data(),
        .signalSemaphoreInfoCount = 1,
        .pSignalSemaphoreInfos = &signal_info,
    };
    ThrowIfFailed(vkQueueSubmit2(
        present_queue, 1, &submit_info, VK_NULL_HANDLE),
        "Vulkan: Failed to wait for semaphores");
}

SwapchainStatus SwapchainImpl::QueuePresent(unsigned image_idx) {
    auto pres_sem = GetPresentSemaphore(image_idx);
    VkPresentInfoKHR present_info = {
        .sType = SType(present_info),
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = &pres_sem,
        .swapchainCount = 1,
        .pSwapchains = &swapchain,
        .pImageIndices = &image_idx,
    };
    auto r = vkQueuePresentKHR(present_queue, &present_info);
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
    }
    throw std::runtime_error{
        "Vulkan: Failed to present to swapchain"};
}

void SwapchainImpl::SignalPresentSemaphore(
    unsigned image_idx, const SemaphoreState* signal_state
) {
    // Signal an external timeline semaphore and insert a fence to
    // know when this image's semaphore is safe to reuse.
    // This works because the first sync scope includes all
    // previous commands.
    VkSemaphoreSubmitInfo signal_info = {
        .sType = SType(signal_info),
    };
    VkSubmitInfo2 submit_info = {
        .sType = SType(submit_info),
    };
    if (signal_state) {
        signal_info.semaphore = signal_state->semaphore;
        signal_info.value = signal_state->value;
        submit_info.signalSemaphoreInfoCount = 1,
        submit_info.pSignalSemaphoreInfos = &signal_info;
    }
    ThrowIfFailed(vkQueueSubmit2(
        present_queue, 1, &submit_info, GetPresentFence(image_idx)),
        "Vulkan: Failed to insert present completion fence");
}

void SwapchainImpl::Resize() {
    auto old_handle = Vk::Swapchain{
        device, std::exchange(swapchain, VK_NULL_HANDLE)};
    Clear();
    Init(old_handle.get());
}

void ResizeSwapchain(Swapchain swapchain) {
    swapchain->Resize();
}
}
