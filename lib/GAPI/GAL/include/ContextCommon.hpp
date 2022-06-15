#pragma once
#include "DeviceCommon.hpp"

#include <span>

namespace R1::GAL {
struct QueueConfig {
    QueueFamilyID id;
    unsigned count;
};

struct ContextConfig {
    std::span<const QueueConfig> queue_config;
    bool wsi: 1;
};
}
