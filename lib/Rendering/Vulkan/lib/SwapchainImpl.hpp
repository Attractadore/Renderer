#pragma once
#include "ImageImpl.hpp"
#include "Swapchain.hpp"
#include "VKRAII.hpp"

namespace R1::VK {
struct SurfaceImpl {
    Vk::Surface                                     handle;
    std::unordered_map<Device, SurfaceDescription>  descriptions;
};

struct SwapchainImageImpl: ImageImpl {
    SwapchainImageImpl(ImageImpl img):
        ImageImpl{std::move(img)} {}
    SwapchainImageImpl(SwapchainImageImpl&& other) = default;
    SwapchainImageImpl& operator=(SwapchainImageImpl&& other) = default;
    ~SwapchainImageImpl() {
        (void)image.release();
    }
};

struct SwapchainImpl {
    Vk::Swapchain                   handle;
    std::vector<SwapchainImageImpl> images;
    Queue                           present_queue;
    SurfaceSizeCallback             surface_size_cb;
    SwapchainDescription            description;
};

constexpr VkSwapchainCreateFlagsKHR SwapchainCapabilitiesToVK(
    const SwapchainCapabilities& caps
);
}

#include "SwapchainImpl.inl"
