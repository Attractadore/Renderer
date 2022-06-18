#include "Common/Vector.hpp"
#include "ContextImpl.hpp"
#include "DeviceImpl.hpp"
#include "GAL/Swapchain.hpp"
#include "ImageImpl.hpp"
#include "InstanceImpl.hpp"
#include "SwapchainImpl.hpp"
#include "SyncImpl.hpp"
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
            return ImageImpl {
                .image = image,
            };
        });
    swc->images.assign(v.begin(), v.end());

    auto create_semaphore = [&] {
        return Vk::Semaphore{dev, CreateBinarySemaphore(dev)};
    };
    auto create_fence = [&] (bool signaled) {
        return Vk::Fence{dev, CreateFence(dev, signaled)};
    };
    auto create_signaled_fence = [&] { return create_fence(true); };
    auto create_unsignaled_fence = [&] { return create_fence(false); };

    swc->acquire_idx = 0;
    swc->acquire_semaphores.resize(images.size());
    swc->acquire_fences.resize(images.size());
    swc->present_semaphores.resize(images.size());
    swc->present_fences.resize(images.size());
    std::ranges::generate(
        swc->acquire_semaphores, create_semaphore);
    // Fence 0 will be signaled by AcquireImage
    std::ranges::generate(
        std::views::take(swc->acquire_fences, 1), create_unsignaled_fence);
    // Fences except fence 0 will be waited on by PresentImage 
    std::ranges::generate(
        std::views::drop(swc->acquire_fences, 1), create_signaled_fence);

    std::ranges::generate(
        swc->present_semaphores, create_semaphore);
    std::ranges::generate(
        swc->present_fences, create_signaled_fence);
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
    const SemaphoreState* signal_state
) {
    auto dev        = swapchain->handle.get_device();
    auto idx        = swapchain->acquire_idx;
    auto acq_sem    = swapchain->acquire_semaphores[idx].get();
    auto acq_fence  = swapchain->acquire_fences[idx].get();

    // It is safe to use the current acquire semaphore here
    // because it has already been ensured that it unsignaled,
    // either in PresentImage or Init if this is a new swapchain.
    uint32_t image_idx = 0;
    auto r = vkAcquireNextImageKHR(dev, swapchain->handle.get(),
        UINT64_MAX, acq_sem, nullptr, &image_idx); 
    switch (r) {
        case VK_SUCCESS:
        case VK_SUBOPTIMAL_KHR: {
            // Transform an internal binary semaphore into an
            // external timeline one and insert a fence to know
            // when the current acquire semaphore is safe to reuse.

            VkSemaphoreSubmitInfo wait_info = {
                .sType = sType(wait_info),
                .semaphore = acq_sem,
            };
            VkSemaphoreSubmitInfo signal_info = {
                .sType = sType(signal_info),
            };
            VkSubmitInfo2 submit_info = {
                .sType = sType(submit_info),
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
                swapchain->present_queue, 1, &submit_info, acq_fence),
                "Vulkan: Failed to signal semaphore");

            return {image_idx, SwapchainStatus::Good};
        } case VK_ERROR_OUT_OF_DATE_KHR:
            return {-1, SwapchainStatus::RequiresSlowResize};
        default:
            throw std::runtime_error{
                "Vulkan: Failed to acquire image from swapchain"};
    }
}

SwapchainStatus PresentImage(
    Swapchain swapchain,
    unsigned image_idx,
    std::span<const SemaphoreState> wait_states,
    const SemaphoreState* signal_state
) {
    auto dev = swapchain->handle.get_device();
    auto pres_sem = swapchain->present_semaphores[image_idx].get();
    auto pres_fence = swapchain->present_fences[image_idx].get();

    // Wait for previous presentation of this image to complete
    // before reusing its semaphore.
    ThrowIfFailed(WaitForFenceAndReset(dev, pres_fence),
        "Vulkan: Failed to avoid present semaphore wait hazard");

    {
        // Mux external timeline semaphores into an internal binary
        // one.

        static thread_local std::vector<VkSemaphoreSubmitInfo> wait_infos;

        auto v = std::views::transform(wait_states,
            [] (const SemaphoreState& state) {
                VkSemaphoreSubmitInfo info = {
                    .sType = sType(info),
                    .semaphore = state.semaphore,
                    .value = state.value,
                };
                return info;
            });
        wait_infos.assign(v.begin(), v.end());

        VkSemaphoreSubmitInfo signal_info = {
            .sType = sType(signal_info),
            .semaphore = pres_sem,
        };
        VkSubmitInfo2 submit_info = {
            .sType = sType(submit_info),
            .waitSemaphoreInfoCount =
                static_cast<uint32_t>(wait_infos.size()),
            .pWaitSemaphoreInfos = wait_infos.data(),
            .signalSemaphoreInfoCount = 1,
            .pSignalSemaphoreInfos = &signal_info,
        };
        ThrowIfFailed(vkQueueSubmit2(
            swapchain->present_queue, 1, &submit_info, VK_NULL_HANDLE),
            "Vulkan: Failed to wait for semaphores");
    }

    auto vk_swc = swapchain->handle.get();
    VkPresentInfoKHR present_info = {
        .sType = sType(present_info),
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = &pres_sem,
        .swapchainCount = 1,
        .pSwapchains = &vk_swc,
        .pImageIndices = &image_idx,
    };
    auto r = vkQueuePresentKHR(
        swapchain->present_queue, &present_info);
    auto status = SwapchainStatus::Good;
    switch (r) {
        case VK_SUCCESS:
            break;
        case VK_SUBOPTIMAL_KHR:
        case VK_ERROR_OUT_OF_DATE_KHR: {
            status = SwapchainStatus::RequiresSlowResize;
            break;
        } default:
            throw std::runtime_error{
                "Vulkan: Failed to present to swapchain"};
    }

    {
        // Signal external timeline semaphore and insert a fence to
        // know when this image's semaphore is safe to reuse.
        //
        // This works because the first sync scope includes all
        // previous commands.

        VkSemaphoreSubmitInfo signal_info = {
            .sType = sType(signal_info),
        };
        VkSubmitInfo2 submit_info = {
            .sType = sType(submit_info),
        };
        if (signal_state) {
            signal_info.semaphore = signal_state->semaphore;
            signal_info.value = signal_state->value;
            submit_info.signalSemaphoreInfoCount = 1,
            submit_info.pSignalSemaphoreInfos = &signal_info;
        }
        ThrowIfFailed(vkQueueSubmit2(
            swapchain->present_queue, 1, &submit_info, pres_fence),
            "Vulkan: Failed to insert present completion fence");
    }

    {
        // Wait for the previous acquire to complete before
        // reusing its semaphore.
        //
        // When AcquireImage reports that the swapchain is out of date,
        // it is expected that it will be called again.
        // In this case, the current acquire fence is already unsignaled,
        // so additional logic is required to prevent further waits
        // on it.
        //
        // But PresentImage is called only once, so this conditional
        // logic is avoided if hazard prevention is performed here.

        auto& idx = swapchain->acquire_idx;
        const auto& acq_fences = swapchain->acquire_fences;
        idx = (idx + 1) % acq_fences.size();
        auto acq_fence = acq_fences[idx].get();
        ThrowIfFailed(WaitForFenceAndReset(dev, acq_fence),
            "Vulkan: Failed to avoid acquire semaphore signal hazard");
    }
    
    return status;
}

void ResizeSwapchain(Swapchain swapchain) {
    InitSwapchain(swapchain);
}
}
