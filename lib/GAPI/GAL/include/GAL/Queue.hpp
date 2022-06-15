#pragma once
#include "Instance.hpp"

namespace R1::GAL {
Queue GetQueue(Context ctx, QueueFamily::ID family, unsigned idx);

void QueueSubmit(
    Queue queue,
    std::span<const QueueSubmitConfig> configs,
    Fence fence
);

void QueueWaitIdle(Queue queue);
}
