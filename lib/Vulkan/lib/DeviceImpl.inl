#pragma once

namespace R1::VK {
inline QueueCapabilities QueueCapabilitiesFromVK(VkQueueFlags flags) {
    return {
        .graphics = (flags & VK_QUEUE_GRAPHICS_BIT) != 0,
        .compute = (flags & VK_QUEUE_COMPUTE_BIT) != 0,
        .transfer = (flags & VK_QUEUE_TRANSFER_BIT) != 0,
    };
}
}
