#include "Instance.hpp"

#include <algorithm>

namespace R1::Rendering {
const std::string& Device::GetName() const {
    return GetDescription().name;
}

const DeviceDescription& Device::GetDescription() const {
    return GAPI::GetDeviceDescription(m_device);
}

Instance::Instance(HInstance instance):
    m_instance{std::move(instance)}
{
    auto dev_cnt = GAPI::GetDeviceCount(m_instance.get());
    devices.reserve(dev_cnt);
    for (size_t i = 0; i < dev_cnt; i++) {
        devices.emplace_back(GAPI::GetDevice(m_instance.get(), i));
    }
}
}
