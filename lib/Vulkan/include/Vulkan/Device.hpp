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
}

namespace R1::VK {
struct DeviceImpl {
    VkInstance instance;
    VkPhysicalDevice physical_device;
    DeviceDescription description;
};
using Device = DeviceImpl*;
}
