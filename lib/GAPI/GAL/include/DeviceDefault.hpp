#pragma once

namespace R1::GAL {
namespace Detail {
enum class DefaultQueueCapability {
    Graphics    = 1 << 0,
    Compute     = 1 << 1,
    Transfer    = 1 << 2,
};

enum class DefaultDeviceType {
    Unknown,
    IntegratedGPU,
    DiscreteGPU,
    VirtualGPU,
    CPU,
};
}
}
