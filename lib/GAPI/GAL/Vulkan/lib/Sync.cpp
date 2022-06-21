#include "ContextImpl.hpp"
#include "GAL/Sync.hpp"
#include "VKUtil.hpp"

#include <ranges>

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

VkSemaphore CreateBinarySemaphore(Context ctx) {
    return CreateSemaphoreBase(ctx, VK_SEMAPHORE_TYPE_BINARY);
}

VkFence CreateFence(Context ctx, bool signaled = false) {
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
    static thread_local std::vector<VkSemaphore> wait_semaphores;
    static thread_local std::vector<uint64_t> wait_values;

    auto sv = std::views::transform(wait_states,
        [] (const SemaphoreState& state) {
            return state.semaphore;
        });
    auto vv = std::views::transform(wait_states,
        [] (const SemaphoreState& state) {
            return state.value;
        });

    wait_semaphores.assign(sv.begin(), sv.end());
    wait_values.assign(vv.begin(), vv.end());

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
}
