#pragma once
#include "ImageImpl.hpp"
#include "Swapchain.hpp"
#include "VKRAII.hpp"

namespace R1::VK {
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
    Queue                           present_queue;
    SurfaceSizeCallback             surface_size_cb;
    VkSwapchainCreateInfoKHR        create_info;
};
}

#include "SwapchainImpl.inl"
