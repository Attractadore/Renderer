#pragma once
#include "Device.hpp"

namespace R1::VK {
QueueCapabilities QueueCapabilitiesFromVK(VkQueueFlags flags);
}

#include "DeviceImpl.inl"
