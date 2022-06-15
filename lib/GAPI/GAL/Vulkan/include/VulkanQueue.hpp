#pragma once
#include "VulkanCommand.hpp"
#include "VulkanSync.hpp"

#include <span>

namespace R1::GAL {
using Queue = VkQueue;

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
}
