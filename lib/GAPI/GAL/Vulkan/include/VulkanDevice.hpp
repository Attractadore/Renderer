#pragma once
#include <vulkan/vulkan.h>

namespace R1::GAL {
enum class QueueCapability {
    Graphics    = VK_QUEUE_GRAPHICS_BIT,
    Compute     = VK_QUEUE_COMPUTE_BIT,
    Transfer    = VK_QUEUE_TRANSFER_BIT,
};

enum class DeviceType {
    Unknown         = VK_PHYSICAL_DEVICE_TYPE_OTHER,
    IntegratedGPU   = VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU,
    DiscreteGPU     = VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU,
    VirtualGPU      = VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU,
    CPU             = VK_PHYSICAL_DEVICE_TYPE_CPU,
};

struct DeviceImpl;
using Device = DeviceImpl*;

namespace Vulkan {
VkPhysicalDevice GetVkPhysicalDevice(Device device);
}
}
