#pragma once
#include "Common/Device.hpp"

#include <vulkan/vulkan.h>

namespace R1 {
enum class DeviceType {
    Unknown         = VK_PHYSICAL_DEVICE_TYPE_OTHER,
    IntegratedGPU   = VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU,
    DiscreteGPU     = VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU,
    VirtualGPU      = VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU,
    CPU             = VK_PHYSICAL_DEVICE_TYPE_CPU,
};

enum class QueueCapability {
    Graphics = VK_QUEUE_GRAPHICS_BIT,
    Compute = VK_QUEUE_COMPUTE_BIT,
    Transfer = VK_QUEUE_TRANSFER_BIT,
};
}

namespace R1::VK {
struct DeviceImpl;
using Device = DeviceImpl*;
}
