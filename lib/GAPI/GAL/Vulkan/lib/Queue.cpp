#include "ContextImpl.hpp"
#include "QueueImpl.inl"
#include "VulkanQueue.inl"

#include <algorithm>

namespace R1::GAL {
Queue GetQueue(Context ctx, QueueFamily::ID family, unsigned idx) {
    VkQueue queue;
    ctx->GetDeviceQueue(static_cast<uint32_t>(family), idx, &queue);
    return queue;
}

void Vulkan::QueueSubmit(
    Context ctx, Queue queue, std::span<const QueueSubmitConfig> configs, Fence fence
) {
    static thread_local std::vector<VkSemaphoreSubmitInfo>      semaphore_wait_submits;
    static thread_local std::vector<VkSemaphoreSubmitInfo>      semaphore_signal_submits;
    static thread_local std::vector<VkCommandBufferSubmitInfo>  cmd_buffer_submits;
    static thread_local std::vector<VkSubmitInfo2>              submits;

    semaphore_wait_submits.clear();
    semaphore_signal_submits.clear();
    cmd_buffer_submits.clear();
    std::ranges::for_each(configs,
        [&] (const QueueSubmitConfig& config) {
            auto wv = ranges::views::transform(
                config.wait_semaphores, SemaphoreSubmitToVK);
            auto sv = ranges::views::transform(
                config.signal_semaphores, SemaphoreSubmitToVK);
            auto cv = ranges::views::transform(
                config.command_buffers, CommandBufferSubmitToVK);
            semaphore_wait_submits.insert(semaphore_wait_submits.end(),
                    wv.begin(), wv.end());
            semaphore_signal_submits.insert(semaphore_signal_submits.end(),
                    sv.begin(), sv.end());
            cmd_buffer_submits.insert(cmd_buffer_submits.end(),
                    cv.begin(), cv.end());
        });

    auto wp = semaphore_wait_submits.data();
    auto sp = semaphore_signal_submits.data();
    auto cp = cmd_buffer_submits.data();
    auto v = ranges::views::transform(configs,
        [&] (const QueueSubmitConfig& config) {
            unsigned wait_cnt = config.wait_semaphores.size();
            unsigned signal_cnt = config.signal_semaphores.size();
            unsigned cmd_cnt = config.command_buffers.size();
            VkSubmitInfo2 info = {
                .sType = SType(info),
                .waitSemaphoreInfoCount = wait_cnt,
                .pWaitSemaphoreInfos = wp,
                .commandBufferInfoCount = cmd_cnt,
                .pCommandBufferInfos = cp,
                .signalSemaphoreInfoCount = signal_cnt,
                .pSignalSemaphoreInfos = sp,
            };
            wp += wait_cnt;
            sp += signal_cnt;
            cp += cmd_cnt;
            return info;
        });
    submits.assign(v.begin(), v.end());

    ThrowIfFailed(
        ctx->QueueSubmit2(queue, submits.size(), submits.data(), fence),
        "Vulkan: Failed to submit work to queue");
};

void QueueWaitIdle(Context ctx, Queue queue) {
    ThrowIfFailed(
        ctx->QueueWaitIdle(queue),
        "Vulkan: Failed to wait for idle queue");
}
}
