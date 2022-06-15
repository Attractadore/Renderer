#pragma once
#include "DeviceCommon.hpp"
#include "InstanceCommon.hpp"
#include "Traits.hpp"

namespace R1::GAL {
using QueueFamily = Detail::QueueFamilyBase<Detail::Traits>;
using DeviceDescription = Detail::DeviceDescriptionBase<Detail::Traits>;

InstanceDescription GetInstanceDescription();
void DestroyInstance(Instance instance);

size_t GetDeviceCount(Instance instance);
Device GetDevice(Instance instance, size_t idx = 0);

const DeviceDescription& GetDeviceDescription(Device dev);
}
