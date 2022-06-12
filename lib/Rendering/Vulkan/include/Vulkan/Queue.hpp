#pragma once
#include "Common/Queue.hpp"
#include "Command.hpp"
#include "Sync.hpp"

namespace R1::VK {
using Queue = VkQueue;
Queue GetQueue(Context ctx, QueueFamily::ID family, unsigned idx);

struct SemaphoreSubmitConfig: VkSemaphoreSubmitInfo {
    SemaphoreSubmitConfig(Semaphore sem, PipelineStageFlags stages);
};

struct CommandBufferSubmitConfig: VkCommandBufferSubmitInfo {
    CommandBufferSubmitConfig(CommandBuffer cmd_buffer);
};

struct QueueSubmitConfig: VkSubmitInfo2 {
    QueueSubmitConfig(
        std::span<const SemaphoreSubmitConfig> wait_semaphores,
        std::span<const CommandBufferSubmitConfig> cmd_buffers,
        std::span<const SemaphoreSubmitConfig> signal_semaphores
    );
};

void QueueSubmit(
    Queue queue,
    std::span<const QueueSubmitConfig> configs,
    Fence fence
);

void QueueWaitIdle(Queue queue);
}
