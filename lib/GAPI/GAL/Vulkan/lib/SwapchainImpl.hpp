#pragma once
#include "ImageImpl.hpp"
#include "GAL/Swapchain.hpp"
#include "VKRAII.hpp"

namespace R1::GAL {
struct SurfaceImpl {
    Vk::Surface                                     handle;
    std::unordered_map<Device, SurfaceDescription>  descriptions;
};

Surface CreateSurfaceFromHandle(Instance instance, Vk::Surface handle);

class SwapchainImpl {
    VkPhysicalDevice            adapter;
    VkDevice                    device;
    VkSwapchainKHR              swapchain;
    struct ImageData {
        ImageImpl               handle;
        VkSemaphore             acquire_semaphore;
        VkFence                 acquire_fence;
        VkSemaphore             present_semaphore;
        VkFence                 present_fence;
    };
    std::vector<ImageData>      images;
    Queue                       present_queue;
    SurfaceSizeCallback         surface_size_cb;
    VkSwapchainCreateInfoKHR    create_info;
    unsigned                    acquire_idx;

public:
    SwapchainImpl(
        VkPhysicalDevice pdev,
        VkDevice dev,
        VkSurfaceKHR surf,
        SurfaceSizeCallback size_cb,
        const SwapchainConfig& config
    );
    void Init(VkSwapchainKHR old_swapchain = VK_NULL_HANDLE);

    ~SwapchainImpl();

    unsigned ImageCount() const { return images.size(); }
    Image GetImage(unsigned idx) { return &images[idx].handle; }
    std::tuple<unsigned, unsigned> Size() const;

    std::tuple<unsigned, SwapchainStatus> AcquireImage(
        const SemaphoreState* signal_state
    );
    SwapchainStatus PresentImage(
        unsigned image_idx,
        std::span<const SemaphoreState> wait_states,
        const SemaphoreState* signal_state
    );
    void Resize();

private:
    void Clear();

    VkSemaphore GetCurrentAcquireSemaphore() const noexcept { return images[acquire_idx].acquire_semaphore; }
    VkFence     GetCurrentAcquireFence() const noexcept { return images[acquire_idx].acquire_fence; }
    VkSemaphore GetPresentSemaphore(unsigned image_idx) const noexcept { return images[image_idx].present_semaphore; }
    VkFence     GetPresentFence(unsigned image_idx) const noexcept { return images[image_idx].present_fence; }

    void WaitOnAcquireFence();
    void SignalAcquireSemaphore(const SemaphoreState* signal_state);

    void WaitOnPresentFence(unsigned image_idx);
    void MuxPresentSemaphores(
        unsigned image_idx, std::span<const SemaphoreState> wait_states
    );
    SwapchainStatus QueuePresent(unsigned image_idx);
    void SignalPresentSemaphore(
        unsigned image_idx, const SemaphoreState* signal_state
    );
};
}
