#pragma once
#if GAL_USE_VULKAN
#include "VulkanContext.hpp"
#endif

#include "Instance.hpp"

#include <span>

namespace R1::GAL {
struct QueueConfig {
    QueueFamilyID id;
    unsigned count;
};

struct ContextConfig {
    std::span<const QueueConfig> queue_config;
};

void DestroyContext(Context ctx);

void ContextWaitIdle(Context ctx);
}
