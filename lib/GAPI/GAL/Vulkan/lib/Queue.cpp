#include "CommandImpl.hpp"
#include "ContextImpl.hpp"
#include "QueueImpl.inl"
#include "VKUtil.hpp"

#include <numeric>
#include <ranges>

namespace R1::GAL {
Queue GetQueue(Context ctx, QueueFamily::ID family, unsigned idx) {
    VkQueue queue = VK_NULL_HANDLE;
    vkGetDeviceQueue(
        ctx->device.get(), static_cast<uint32_t>(family), idx, &queue);
    return queue;
}

void QueueSubmit(
    Queue queue, std::span<const QueueSubmitConfig> configs
) {
    static thread_local std::vector<VkSemaphoreSubmitInfo>
        semaphore_wait_submits;
    static thread_local std::vector<VkSemaphoreSubmitInfo>
        semaphore_signal_submits;
    static thread_local std::vector<VkCommandBufferSubmitInfo>
        cmd_buffer_submits;
    static thread_local std::vector<VkSubmitInfo2>
        submits;

    auto sem_wait_cnt = [&] {
        auto v = std::views::transform(configs,
            [] (const QueueSubmitConfig& config) {
                return config.wait_semaphores.size();
            });
        return std::accumulate(v.begin(), v.end(), 0);
    } ();

    auto sem_signal_cnt = [&] {
        auto v = std::views::transform(configs,
            [] (const QueueSubmitConfig& config) {
                return config.signal_semaphores.size();
            });
        return std::accumulate(v.begin(), v.end(), 0);
    } ();

    auto cmd_buffer_cnt = [&] {
        auto v = std::views::transform(configs,
            [] (const QueueSubmitConfig& config) {
                return config.command_buffers.size();
            });
        return std::accumulate(v.begin(), v.end(), 0);
    } ();

    semaphore_wait_submits.clear();
    semaphore_wait_submits.reserve(sem_wait_cnt);
    semaphore_signal_submits.clear();
    semaphore_signal_submits.reserve(sem_signal_cnt);
    cmd_buffer_submits.clear();
    cmd_buffer_submits.reserve(cmd_buffer_cnt);

    auto v = std::views::transform(configs,
        [&] (const QueueSubmitConfig& config) {
            auto wv = std::views::transform(
                config.wait_semaphores, SemaphoreSubmitToVK);
            auto sv = std::views::transform(
                config.signal_semaphores, SemaphoreSubmitToVK);
            auto cv = std::views::transform(
                config.command_buffers, CommandBufferSubmitToVK);
            VkSubmitInfo2 info {
                .sType = sType(info),
                .waitSemaphoreInfoCount =
                    static_cast<uint32_t>(wv.size()),
                .pWaitSemaphoreInfos =
                    semaphore_wait_submits.data() +
                    semaphore_wait_submits.size(),
                .commandBufferInfoCount =
                    static_cast<uint32_t>(cv.size()),
                .pCommandBufferInfos =
                    cmd_buffer_submits.data() +
                    cmd_buffer_submits.size(),
                .signalSemaphoreInfoCount =
                    static_cast<uint32_t>(sv.size()),
                .pSignalSemaphoreInfos =
                    semaphore_signal_submits.data() +
                    semaphore_signal_submits.size(),
            };
            semaphore_wait_submits.insert(semaphore_wait_submits.end(),
                    wv.begin(), wv.end());
            semaphore_signal_submits.insert(semaphore_signal_submits.end(),
                    sv.begin(), sv.end());
            cmd_buffer_submits.insert(cmd_buffer_submits.end(),
                    cv.begin(), cv.end());
            return info;
    });

    submits.assign(v.begin(), v.end());

    auto r = vkQueueSubmit2(
        queue, submits.size(), submits.data(), VK_NULL_HANDLE);
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
