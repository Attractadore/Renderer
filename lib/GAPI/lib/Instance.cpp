#include "Instance.hpp"

namespace R1::Rendering {
const std::string& Device::GetName() const {
    return GetDescription().name;
}

const DeviceDescription& Device::GetDescription() const {
    return GAPI::GetDeviceDescription(m_device);
}

QueueFamily::ID Device::FindQueueFamilyWithCapabilities(
    QueueCapabilityFlags caps, QueueCapabilityFlags filter
) const noexcept {
    for (const auto& qf: GetDescription().queue_families) {
        if ((qf.capabilities & filter) == caps) {
            return qf.id;
        }
    }
    return QueueFamily::ID::Unknown;
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
