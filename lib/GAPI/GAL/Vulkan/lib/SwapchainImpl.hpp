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

struct SwapchainImpl {
    VkPhysicalDevice                adapter;
    Vk::Swapchain                   handle;
    std::vector<ImageImpl>          images;
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
