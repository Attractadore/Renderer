#pragma once
#include "Instance.decl.hpp"

namespace R1::VK {
inline void DestroyInstance(Instance instance) {
    delete instance;
}

inline size_t GetDeviceCount(Instance instance) {
    return instance->devices.size();
}

inline Device GetDevice(Instance instance, size_t idx) {
    assert(idx < GetDeviceCount(instance));
    return &instance->devices[idx];
}

inline const DeviceDescription& GetDeviceDescription(Device dev) {
    return dev->description;
}
}
