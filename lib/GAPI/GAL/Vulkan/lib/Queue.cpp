#include "ContextImpl.hpp"
#include "QueueImpl.inl"
#include "VKUtil.hpp"
#include "VulkanQueue.inl"

namespace R1::GAL {
Queue GetQueue(Context ctx, QueueFamily::ID family, unsigned idx) {
    VkQueue queue;
    ctx->GetDeviceQueue(static_cast<uint32_t>(family), idx, &queue);
    return queue;
}

void Vulkan::QueueSubmit(
    Context ctx, Queue queue, std::span<const QueueSubmitConfig> configs, Fence fence
) {
    DefaultSmallVector<VkSemaphoreSubmitInfo>       semaphore_submits;
    DefaultSmallVector<VkCommandBufferSubmitInfo>   cmd_buffer_submits;
    DefaultSmallVector<VkSubmitInfo2>               submits(configs.size());

    semaphore_submits.reserve(
        ranges::accumulate(configs, 0, std::plus{},
        [](const QueueSubmitConfig& config) {
            return config.wait_semaphores.size() +
                config.signal_semaphores.size();
        })
    );
    cmd_buffer_submits.reserve(
        ranges::accumulate(configs, 0, std::plus{},
        [](const QueueSubmitConfig& config) {
            return config.command_buffers.size();
        })
    );

    std::ranges::transform(configs, submits.begin(),
        [&] (const QueueSubmitConfig& config) {
            auto wv = ranges::views::transform(
                config.wait_semaphores, SemaphoreSubmitToVK);
            auto sv = ranges::views::transform(
                config.signal_semaphores, SemaphoreSubmitToVK);
            auto cv = ranges::views::transform(
                config.command_buffers, CommandBufferSubmitToVK);

            auto old_wdata = semaphore_submits.data();
            auto wit = semaphore_submits.append(wv);
            assert(old_wdata == semaphore_submits.data());

            auto old_sdata = semaphore_submits.data();
            auto sit = semaphore_submits.append(sv);
            assert(old_sdata == semaphore_submits.data());

            auto old_cdata = cmd_buffer_submits.data();
            auto cit = cmd_buffer_submits.append(cv);
            assert(old_cdata == cmd_buffer_submits.data());

            VkSubmitInfo2 info = {
                .sType = SType(info),
                .waitSemaphoreInfoCount = static_cast<uint32_t>(config.wait_semaphores.size()),
                .pWaitSemaphoreInfos = &*wit,
                .commandBufferInfoCount = static_cast<uint32_t>(config.command_buffers.size()),
                .pCommandBufferInfos = &*cit,
                .signalSemaphoreInfoCount = static_cast<uint32_t>(config.signal_semaphores.size()),
                .pSignalSemaphoreInfos = &*sit,
            };

            return info;
        });

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
