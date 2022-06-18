#pragma once

namespace R1::GAL {
enum class SemaphoreStatus {
    Ready,
    NotReady,
};

namespace Detail {
template<typename Traits>
struct SemaphoreConfigBase {
    Traits::SemaphorePayload initial_value;
};

template<typename Traits>
struct SemaphoreStateBase {
    Traits::Semaphore           semaphore;
    Traits::SemaphorePayload    value;
};
}
}
