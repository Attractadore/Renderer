#include "ContextImpl.hpp"
#include "GAL/Sync.hpp"
#include "VKUtil.hpp"

namespace R1::GAL {
Fence CreateFence(Context ctx, const FenceConfig& config) {
    VkFenceCreateInfo create_info = {
        .sType = sType(create_info),
        .flags = config.signaled ? VK_FENCE_CREATE_SIGNALED_BIT : 0u,
    };
    VkFence fence = VK_NULL_HANDLE;
    vkCreateFence(ctx->device.get(), &create_info, nullptr, &fence);
    if (!fence) {
        throw std::runtime_error{"Vulkan: Failed to create fence"};
    }
    return fence;
}

void DestroyFence(Context ctx, Fence fence) {
    vkDestroyFence(ctx->device.get(), fence, nullptr);
}

FenceStatus GetFenceStatus(Context ctx, Fence fence) {
    auto r = vkGetFenceStatus(ctx->device.get(), fence);
    switch(r) {
        case VK_SUCCESS:
            return FenceStatus::Ready;
        case VK_NOT_READY:
            return FenceStatus::NotReady;
        default: {
            throw std::runtime_error{
                "Vulkan: Failed to get fence status"};
        }
    }
}

FenceStatus WaitForFences(
    Context ctx, std::span<const Fence> fences, bool all, std::chrono::nanoseconds timeout
) {
    auto r = vkWaitForFences(
        ctx->device.get(), fences.size(), fences.data(), all, timeout.count());
    switch(r) {
        case VK_SUCCESS:
            return FenceStatus::Ready;
        case VK_TIMEOUT:
            return FenceStatus::NotReady;
        default: {
            throw std::runtime_error{
                "Vulkan: Failed to wait for fences"};
        }
    }
}

void ResetFences(Context ctx, std::span<const Fence> fences) {
    auto r = vkResetFences(
        ctx->device.get(), fences.size(), fences.data());
    if (r) {
        throw std::runtime_error{"Vulkan: Failed to reset fences"};
    }
}

Semaphore CreateSemaphore(Context ctx) {
    VkSemaphoreCreateInfo create_info = {
        .sType = sType(create_info),
    };
    VkSemaphore sem = VK_NULL_HANDLE;
    vkCreateSemaphore(ctx->device.get(), &create_info, nullptr, &sem);
    if (!sem) {
        throw std::runtime_error{"Vulkan: Failed to create semaphore"};
    }
    return sem;
}

void DestroySemaphore(Context ctx, Semaphore sem) {
    vkDestroySemaphore(ctx->device.get(), sem, nullptr);
}
}
