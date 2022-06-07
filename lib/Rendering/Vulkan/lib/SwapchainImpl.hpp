#pragma once
#include "ImageImpl.hpp"
#include "Swapchain.hpp"
#include "VKRAII.hpp"

namespace R1::VK {
struct SwapchainImpl {
    Vk::Swapchain           handle;
    std::vector<ImageImpl>  images;
    Queue                   present_queue;
    SurfaceSizeCallback     surface_size_cb;
    SwapchainDescription    description;
};

constexpr VkSwapchainCreateFlagsKHR SwapchainCapabilitiesToVK(
    const SwapchainCapabilities& caps
);
}

#include "SwapchainImpl.inl"
