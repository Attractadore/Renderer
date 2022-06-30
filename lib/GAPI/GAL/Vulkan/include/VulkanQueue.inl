#pragma once
#include "GAL/Queue.hpp"

namespace R1::GAL {
namespace Vulkan {
void QueueSubmit(
    Context ctx, Queue queue,
    std::span<const QueueSubmitConfig> configs,
    Fence fence
);
}

inline void QueueSubmit(
    Context ctx, Queue queue,
    std::span<const QueueSubmitConfig> configs
) {
    Vulkan::QueueSubmit(ctx, queue, configs, nullptr);
}
}
