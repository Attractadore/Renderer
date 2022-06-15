#include "Instance.hpp"

namespace R1::GAPI {
const std::string& Device::GetName() const {
    return GetDescription().name;
}

const GAL::DeviceDescription& Device::GetDescription() const {
    return GAL::GetDeviceDescription(m_device);
}

GAL::QueueFamily::ID Device::FindQueueFamilyWithCapabilities(
    GAL::QueueCapabilityFlags caps, GAL::QueueCapabilityFlags filter
) const noexcept {
    for (const auto& qf: GetDescription().queue_families) {
        if ((qf.capabilities & filter) == caps) {
            return qf.id;
        }
    }
    return GAL::QueueFamily::ID::Unknown;
}

Instance::Instance(HInstance instance):
    m_instance{std::move(instance)}
{
    auto dev_cnt = GAL::GetDeviceCount(m_instance.get());
    devices.reserve(dev_cnt);
    for (size_t i = 0; i < dev_cnt; i++) {
        devices.emplace_back(GAL::GetDevice(m_instance.get(), i));
    }
}
}
