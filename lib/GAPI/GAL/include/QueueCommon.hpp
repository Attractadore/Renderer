#pragma once
#include "SyncCommon.hpp"

#include <span>

namespace R1::GAL {
namespace Detail {
template<typename Traits>
struct SemaphoreSubmitConfigBase {
    SemaphoreStateBase<Traits>  state;
    Traits::PipelineStageFlags  stages;
};

template<typename Traits>
struct QueueSubmitConfigBase {
    std::span<const SemaphoreSubmitConfigBase<Traits>>  wait_semaphores; 
    std::span<const SemaphoreSubmitConfigBase<Traits>>  signal_semaphores;
    std::span<const typename Traits::CommandBuffer>     command_buffers;
};
}
}
