#pragma once
#include "Common/Sync.hpp"
#include "Context.hpp"

#include <chrono>

namespace R1::VK {
using Fence = VkFence;
using Semaphore = VkSemaphore;

Fence CreateFence(Context ctx, const FenceConfig& config);
void DestroyFence(Context ctx, Fence fence);

FenceStatus GetFenceStatus(Context ctx, Fence fence);
FenceStatus WaitForFences(Context ctx, std::span<const Fence> fences, bool all, std::chrono::nanoseconds timeout);
void ResetFences(Context ctx, std::span<const Fence> fences);

Semaphore CreateSemaphore(Context ctx);
void DestroySemaphore(Context ctx, Semaphore sem);
}
