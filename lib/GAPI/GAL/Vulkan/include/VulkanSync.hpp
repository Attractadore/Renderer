#pragma once
#include "VulkanContext.hpp"

#include <chrono>
#include <span>

namespace R1::GAL {
using Semaphore = VkSemaphore;
using SemaphorePayload = uint64_t;

namespace Vulkan {
Semaphore CreateBinarySemaphore(Context ctx);

enum class FenceStatus {
    Ready,
    NotReady,
};

using Fence = VkFence;
Fence CreateFence(Context ctx, bool signaled);
void DestroyFence(Context ctx, Fence fence);

FenceStatus WaitForFences(
    Context ctx,
    std::span<const Fence> fences,
    bool all,
    std::chrono::nanoseconds timeout
);

void ResetFences(Context ctx, std::span<const Fence> fences);
}
}
