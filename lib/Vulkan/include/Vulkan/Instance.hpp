#pragma once
#include "Common/Instance.hpp"
#include "Device.hpp"
#include "VKRAII.hpp"

namespace R1::VK {
struct InstanceImpl {
    Vk::Instance instance;
    std::vector<DeviceImpl> devices;
};
using Instance = InstanceImpl*;

InstanceDescription GetInstanceDescription();
void DestroyInstance(Instance instance);

size_t GetDeviceCount(Instance instance);
Device GetDevice(Instance instance, size_t idx = 0);

const DeviceDescription& GetDeviceDescription(Device dev);
}

#include "Instance.inl"
