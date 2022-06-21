#pragma once
#if GAL_USE_VULKAN
#include "VulkanQueue.hpp"
#endif

#include "Command.hpp"
#include "Sync.hpp"

namespace R1::GAL {
struct SemaphoreSubmitConfig {
    SemaphoreState      state;
    PipelineStageFlags  stages;
};

struct QueueSubmitConfig {
    std::span<const SemaphoreSubmitConfig>  wait_semaphores; 
    std::span<const SemaphoreSubmitConfig>  signal_semaphores;
    std::span<const CommandBuffer>          command_buffers;
};

Queue GetQueue(Context ctx, QueueFamily::ID family, unsigned idx);

void QueueSubmit(
    Context ctx, Queue queue, std::span<const QueueSubmitConfig> configs
);

void QueueWaitIdle(Context ctx, Queue queue);
}
