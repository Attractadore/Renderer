#pragma once
#include "ImageImpl.hpp"
#include "VulkanSwapchain.hpp"
#include "VulkanInstance.hpp"
#include "VKRAII.hpp"

#include <bitset>

namespace R1::GAL {
struct SurfaceImpl {
    Vk::Surface                                     handle;
    std::unordered_map<Device, SurfaceDescription>  descriptions;
};

Surface CreateSurfaceFromHandle(Instance instance, Vk::Surface handle);

struct SwapchainImageImpl: ImageImpl {
    SwapchainImageImpl(ImageImpl img):
        ImageImpl{std::move(img)} {}
    SwapchainImageImpl(const SwapchainImageImpl&) = delete;
    SwapchainImageImpl(SwapchainImageImpl&& other) = default;
    SwapchainImageImpl& operator=(const SwapchainImageImpl&) = delete;
    SwapchainImageImpl& operator=(SwapchainImageImpl&& other) {
        (void)image.release();
        ImageImpl::operator=(std::move(other));
        return *this;
    }
    ~SwapchainImageImpl() {
        (void)image.release();
    }
};

struct SwapchainImpl {
    VkPhysicalDevice                adapter;
    Vk::Swapchain                   handle;
    std::vector<SwapchainImageImpl> images;
    std::vector<Vk::Semaphore>      acquire_semaphores;
    std::vector<Vk::Fence>          acquire_fences;
    std::vector<Vk::Semaphore>      present_semaphores;
    std::vector<Vk::Fence>          present_fences;
    Queue                           present_queue;
    SurfaceSizeCallback             surface_size_cb;
    VkSwapchainCreateInfoKHR        create_info;
    unsigned                        acquire_idx;
};
}

#include "SwapchainImpl.inl"
