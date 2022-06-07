#include "Queue.hpp"
#include "ContextImpl.hpp"
#include "CommandImpl.hpp"
#include "VKUtil.hpp"

namespace R1::VK {
Queue GetQueue(Context ctx, QueueFamily::ID family, unsigned idx) {
    VkQueue queue = VK_NULL_HANDLE;
    vkGetDeviceQueue(
        ctx->device.get(), static_cast<uint32_t>(family), idx, &queue);
    return queue;
}

SemaphoreSubmitConfig::SemaphoreSubmitConfig(
    Semaphore sem, const PipelineStages& stages
): VkSemaphoreSubmitInfo {
    .sType = R1::VK::sType<VkSemaphoreSubmitInfo>(),
    .semaphore = sem,
    .stageMask = PipelineStagesToVK(stages),
} {}

CommandBufferSubmitConfig::CommandBufferSubmitConfig(
    CommandBuffer cmd_buffer
): VkCommandBufferSubmitInfo {
    .sType = R1::VK::sType<VkCommandBufferSubmitInfo>(),
    .commandBuffer = cmd_buffer,
} {}

QueueSubmitConfig::QueueSubmitConfig(
    std::span<const SemaphoreSubmitConfig> wait_semaphores,
    std::span<const CommandBufferSubmitConfig> cmd_buffers,
    std::span<const SemaphoreSubmitConfig> signal_semaphores
): VkSubmitInfo2 {
    .sType = R1::VK::sType<VkSubmitInfo2>(),
    .waitSemaphoreInfoCount =
        static_cast<uint32_t>(wait_semaphores.size()),
    .pWaitSemaphoreInfos = wait_semaphores.data(),
    .commandBufferInfoCount =
        static_cast<uint32_t>(cmd_buffers.size()),
    .pCommandBufferInfos = cmd_buffers.data(),
    .signalSemaphoreInfoCount =
        static_cast<uint32_t>(signal_semaphores.size()),
    .pSignalSemaphoreInfos = signal_semaphores.data(),
} {}

void QueueSubmit(
    Queue queue,
    std::span<const QueueSubmitConfig> configs,
    Fence fence
) {
    static_assert(sizeof(QueueSubmitConfig) == sizeof(VkSubmitInfo2));
    auto r = vkQueueSubmit2(
        queue, configs.size(), configs.data(), fence);
    if (r) {
        throw std::runtime_error{
            "Vulkan: Failed to submit work to queue"};
    }
};

void QueueWaitIdle(Queue queue) {
    auto r = vkQueueWaitIdle(queue);
    if (r) {
        throw std::runtime_error{
            "Vulkan: Failed to wait for idle queue"};
    }
}
}
