#pragma once
#include "QueueImpl.hpp"
#include "VKUtil.hpp"

namespace R1::GAL {
VkSemaphoreSubmitInfo SemaphoreSubmitToVK(const SemaphoreSubmitConfig& config) {
    VkSemaphoreSubmitInfo info = {
        .sType = sType(info),
        .semaphore = config.state.semaphore,
        .value = config.state.value,
        .stageMask = static_cast<VkPipelineStageFlags2>(config.stages.Extract()),
    };
    return info;
};

VkCommandBufferSubmitInfo CommandBufferSubmitToVK(CommandBuffer cmd_buffer) {
    VkCommandBufferSubmitInfo info = {
        .sType = sType(info),
        .commandBuffer = cmd_buffer,
    };
    return info;
}
}
