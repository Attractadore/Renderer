#pragma once
#include "Device.hpp"

namespace R1 {
struct CommandPoolCapabilities {
    bool transient: 1;
    bool reset_command_buffer: 1;
};

struct CommandPoolConfig {
    CommandPoolCapabilities capabilities;
    QueueFamily::ID queue_family;
};

enum class CommandResources {
    Release,
    Keep,
};

struct CommandBufferUsage {
    bool one_time_submit: 1;
    bool simultaneous_use: 1;
};

struct CommandBufferBeginConfig {
    CommandBufferUsage usage;
};
}
