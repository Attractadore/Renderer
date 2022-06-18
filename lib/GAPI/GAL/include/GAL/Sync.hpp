#pragma once
#if GAL_USE_VULKAN
#include "VulkanSync.hpp"
#endif

#include "Context.hpp"

#include <chrono>

namespace R1::GAL {
enum class SemaphoreStatus {
    Ready,
    NotReady,
};

struct SemaphoreConfig {
    SemaphorePayload initial_value;
};

struct SemaphoreState {
    Semaphore           semaphore;
    SemaphorePayload    value;
};

Semaphore CreateSemaphore(Context ctx, const SemaphoreConfig& config);
void DestroySemaphore(Context ctx, Semaphore semaphore);

SemaphorePayload GetSemaphorePayloadValue(
    Context ctx, Semaphore semaphore
);
SemaphoreStatus WaitForSemaphores(
    Context ctx,
    std::span<const SemaphoreState> wait_states,
    bool for_all, std::chrono::nanoseconds timeout
);
void SignalSemaphore(
    Context ctx, const SemaphoreState& signal_state 
);
}
