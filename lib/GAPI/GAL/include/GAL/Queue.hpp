#pragma once
#include "Types.hpp"

namespace R1::GAL::VK {
Queue GetQueue(Context ctx, QueueFamily::ID family, unsigned idx);

void QueueSubmit(
    Queue queue,
    std::span<const QueueSubmitConfig> configs,
    Fence fence
);

void QueueWaitIdle(Queue queue);
}
