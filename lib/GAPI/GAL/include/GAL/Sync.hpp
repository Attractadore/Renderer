#pragma once
#include "SyncCommon.hpp"
#include "Traits.hpp"

#include <chrono>

namespace R1::GAL {
using SemaphoreConfig = Detail::SemaphoreConfigBase<Detail::Traits>;
using SemaphoreState = Detail::SemaphoreStateBase<Detail::Traits>;

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
