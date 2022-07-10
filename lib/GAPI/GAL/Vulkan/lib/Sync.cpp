#include "ContextImpl.hpp"
#include "GAL/Sync.hpp"
#include "VKUtil.hpp"

#include <algorithm>

namespace R1::GAL {
namespace {
VkSemaphore CreateSemaphoreBase(
    Context ctx, VkSemaphoreType type, uint64_t initial_value = 0
) {
    VkSemaphoreTypeCreateInfo type_info = {
        .sType = SType(type_info),
        .semaphoreType = type,
        .initialValue = initial_value,
    };
    VkSemaphoreCreateInfo create_info = {
        .sType = SType(create_info),
        .pNext = &type_info,
    };
    VkSemaphore sem;
    ThrowIfFailed(
        ctx->CreateSemaphore(&create_info, &sem),
        "Vulkan: Failed to create semaphore");
    return sem;
}
}

Semaphore CreateSemaphore(Context ctx, const SemaphoreConfig& config) {
    return CreateSemaphoreBase(
        ctx, VK_SEMAPHORE_TYPE_TIMELINE, config.initial_value);
}

Semaphore Vulkan::CreateBinarySemaphore(Context ctx) {
    return CreateSemaphoreBase(ctx, VK_SEMAPHORE_TYPE_BINARY);
}

void DestroySemaphore(Context ctx, Semaphore sem) {
    ctx->DestroySemaphore(sem);
}

SemaphorePayload GetSemaphorePayloadValue(
    Context ctx, Semaphore semaphore
) {
    uint64_t value;
    ThrowIfFailed(
        ctx->GetSemaphoreCounterValue(semaphore, &value),
        "Vulkan: Failed to get semaphore payload value");
    return value;
}

SemaphoreStatus WaitForSemaphores(
    Context ctx, std::span<const SemaphoreState> wait_states,
    bool for_all, std::chrono::nanoseconds timeout
) {
    DefaultSmallVector<VkSemaphore> wait_semaphores(wait_states.size());
    DefaultSmallVector<uint64_t> wait_values(wait_states.size());

    std::ranges::transform(wait_states, wait_semaphores.begin(),
        [] (const SemaphoreState& state) {
            return state.semaphore;
        });
    std::ranges::transform(wait_states, wait_values.begin(),
        [] (const SemaphoreState& state) {
            return state.value;
        });

    VkSemaphoreWaitInfo wait_info = {
        .sType = SType(wait_info),
        .flags = for_all ? 0u: VK_SEMAPHORE_WAIT_ANY_BIT,
        .semaphoreCount =
            static_cast<uint32_t>(wait_semaphores.size()),
        .pSemaphores = wait_semaphores.data(),
        .pValues = wait_values.data(),
    };
    auto r = ctx->WaitSemaphores(&wait_info, timeout.count());
    switch (r) {
        case VK_SUCCESS:
            return SemaphoreStatus::Ready;
        case VK_TIMEOUT:
            return SemaphoreStatus::NotReady;
        default:
            throw std::runtime_error{
                "Vulkan: Failed to wait for semaphores"};
    }
}

void SignalSemaphore(Context ctx, const SemaphoreState& signal_state) {
    VkSemaphoreSignalInfo signal_info = {
        .sType = SType(signal_info),
        .semaphore = signal_state.semaphore,
        .value = signal_state.value,
    };
    ThrowIfFailed(
        ctx->SignalSemaphore(&signal_info),
        "Vulkan: Failed to signal semaphore");
}

Vulkan::Fence Vulkan::CreateFence(Context ctx, bool signaled = false) {
    VkFenceCreateInfo create_info = {
        .sType = SType(create_info),
        .flags = signaled ? VK_FENCE_CREATE_SIGNALED_BIT: 0u,
    };
    VkFence fence;
    ThrowIfFailed(
        ctx->CreateFence(&create_info, &fence),
        "Vulkan: Failed to create fence"
    );
    return fence;
}

void Vulkan::DestroyFence(Context ctx, Fence fence) {
    ctx->DestroyFence(fence);
}

Vulkan::FenceStatus Vulkan::WaitForFences(
    Context ctx,
    std::span<const Fence> fences,
    bool all,
    std::chrono::nanoseconds timeout
) {
    auto r = ctx->WaitForFences(
        fences.size(), fences.data(), all, timeout.count());
    using enum Vulkan::FenceStatus;
    switch (r) {
        case VK_SUCCESS:
            return Ready;
        case VK_TIMEOUT:
            return NotReady;
        default:
            throw std::runtime_error{
                "Vulkan: Failed to wait for fences"};
    }
}

void Vulkan::ResetFences(Context ctx, std::span<const Fence> fences) {
    ThrowIfFailed(
        ctx->ResetFences(fences.size(), fences.data()),
        "Vulkan: Failed to reset fences");
}
}
