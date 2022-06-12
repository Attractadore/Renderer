#pragma once
#include "Device.hpp"

namespace R1::VK {
struct VKDeviceDescription {
    DeviceDescription common;
    uint32_t api_version;
};
struct DeviceImpl {
    VkInstance instance;
    VkPhysicalDevice physical_device;
    VKDeviceDescription description;
};

QueueCapabilityFlags QueueCapabilitiesFromVK(VkQueueFlags flags);
}

#include "DeviceImpl.inl"
