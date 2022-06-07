#pragma once
#include "Device.hpp"

#include <span>

namespace R1 {
struct QueueConfig {
    QueueFamily::ID id;
    unsigned count;
};

struct ContextConfig {
    std::span<const QueueConfig> queue_config;
    bool wsi: 1 = false;
};
}
