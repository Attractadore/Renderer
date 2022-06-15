#pragma once
#include "Types.hpp"

namespace R1::GAL {
InstanceDescription GetInstanceDescription();
void DestroyInstance(Instance instance);

size_t GetDeviceCount(Instance instance);
Device GetDevice(Instance instance, size_t idx = 0);

const DeviceDescription& GetDeviceDescription(Device dev);
}
