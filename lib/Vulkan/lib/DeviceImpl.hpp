#pragma once
#include "Device.hpp"

namespace R1::VK {
struct DeviceImpl {
    VkInstance instance;
    VkPhysicalDevice physical_device;
    DeviceDescription description;
};

QueueCapabilities QueueCapabilitiesFromVK(VkQueueFlags flags);
}

#include "DeviceImpl.inl"
