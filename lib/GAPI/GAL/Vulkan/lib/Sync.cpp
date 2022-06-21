#include "ContextImpl.hpp"
#include "GAL/Sync.hpp"
#include "VKUtil.hpp"

#include <ranges>

namespace R1::GAL {
namespace {
VkSemaphore CreateSemaphoreBase(
    VkDevice dev, VkSemaphoreType type, uint64_t initial_value = 0
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
        vkCreateSemaphore(dev, &create_info, nullptr, &sem),
        "Vulkan: Failed to create semaphore");
    return sem;
}
}

Semaphore CreateSemaphore(Context ctx, const SemaphoreConfig& config) {
    return CreateSemaphoreBase(
        ctx->device.get(), VK_SEMAPHORE_TYPE_TIMELINE, config.initial_value);
}

VkSemaphore CreateBinarySemaphore(VkDevice dev) {
    return CreateSemaphoreBase(dev, VK_SEMAPHORE_TYPE_BINARY);
}

VkFence CreateFence(VkDevice dev, bool signaled = false) {
    VkFenceCreateInfo create_info = {
        .sType = SType(create_info),
        .flags = signaled ? VK_FENCE_CREATE_SIGNALED_BIT: 0u,
    };
    VkFence fence;
    ThrowIfFailed(
        vkCreateFence(dev, &create_info, nullptr, &fence),
        "Vulkan: Failed to create fence"
    );
    return fence;
}

void DestroySemaphore(Context ctx, Semaphore sem) {
    vkDestroySemaphore(ctx->device.get(), sem, nullptr);
}

SemaphorePayload GetSemaphorePayloadValue(
    Context ctx, Semaphore semaphore
) {
    uint64_t value;
    auto r = vkGetSemaphoreCounterValue(
        ctx->device.get(), semaphore, &value);
    if (r) {
        throw std::runtime_error{
            "Vulkan: Failed to get semaphore payload value"};
    }
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
    auto r = vkWaitSemaphores(
        ctx->device.get(), &wait_info, timeout.count());
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
    auto r = vkSignalSemaphore(ctx->device.get(), &signal_info);
    if (r) {
        throw std::runtime_error{
            "Vulkan: Failed to signal semaphore"};
    }
}
}
