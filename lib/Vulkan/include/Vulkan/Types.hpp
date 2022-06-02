#pragma once
#include "Common/Types.hpp"
#include "VKRAII.hpp"

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

struct InstanceImpl {
    Vk::Instance instance;
    std::vector<DeviceImpl> devices;
};
using Instance = InstanceImpl*;

struct ContextImpl {
    Vk::Device device;
};
using Context = ContextImpl*;

using ShaderModule = VkShaderModule;

using PipelineLayout = VkPipelineLayout;
};
