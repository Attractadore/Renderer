#pragma once
#include <span>

namespace R1 {
template<class SemaphoreSubmitConfig, class CommandBufferSubmitConfig>
struct QueueSubmitConfigBase {
    std::span<const SemaphoreSubmitConfig> wait_semaphores;
    std::span<const CommandBufferSubmitConfig> cmd_buffers;
    std::span<const SemaphoreSubmitConfig> signal_semaphores;
};
};
