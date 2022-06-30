#pragma once
#include "R1VulkanContext.hpp"
#include "R1VulkanInstance.hpp"
#include "R1VulkanScene.hpp"
#include "R1VulkanSwapchain.hpp"

namespace R1 {
inline VulkanInstance* ToPrivate(R1Instance* instance) {
    return static_cast<VulkanInstance*>(instance);
}

inline VulkanContext* ToPrivate(R1Context* ctx) {
    return static_cast<VulkanContext*>(ctx);
}

inline VulkanSwapchain* ToPrivate(R1Swapchain* swapchain) {
    return static_cast<VulkanSwapchain*>(swapchain);
}

inline VulkanScene* ToPrivate(R1Scene* scene) {
    return static_cast<VulkanScene*>(scene);
}
}
