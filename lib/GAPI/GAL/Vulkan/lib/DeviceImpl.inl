#pragma once

namespace R1::GAL {
inline QueueCapabilityFlags QueueCapabilitiesFromVK(VkQueueFlags vk_flags) {
    QueueCapabilityFlags flags = static_cast<QueueCapability>(vk_flags);
    if (flags.AnySet(QueueCapability::Graphics | QueueCapability::Compute)) {
        flags |= QueueCapability::Transfer;
    }
    return flags;
}
}
