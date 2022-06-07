#pragma once
#include "Common/Instance.hpp"
#include "Device.hpp"

namespace R1::VK {
struct InstanceImpl;
using Instance = InstanceImpl*;

InstanceDescription GetInstanceDescription();
void DestroyInstance(Instance instance);

size_t GetDeviceCount(Instance instance);
Device GetDevice(Instance instance, size_t idx = 0);

const DeviceDescription& GetDeviceDescription(Device dev);
}
