#pragma once
#include "GALRAII.hpp"

namespace R1::GAPI {
class Device {
    GAL::Device m_device;

public:
    Device(GAL::Device device):
        m_device{device} {}

    const std::string& GetName() const;
    const GAL::DeviceDescription& GetDescription() const;

    GAL::QueueFamily::ID FindQueueFamilyWithCapabilities(
        GAL::QueueCapabilityFlags caps,
        GAL::QueueCapabilityFlags filter =
            GAL::QueueCapability::Graphics |
            GAL::QueueCapability::Compute |
            GAL::QueueCapability::Transfer
    ) const noexcept;
    
    GAL::QueueFamily::ID FindGraphicsQueueFamily() const noexcept {
        return FindQueueFamilyWithCapabilities(
            GAL::QueueCapability::Graphics |
            GAL::QueueCapability::Compute |
            GAL::QueueCapability::Transfer
        );
    }
    
    GAL::QueueFamily::ID FindComputeQueueFamily() const noexcept {
        return FindQueueFamilyWithCapabilities(
            GAL::QueueCapability::Compute |
            GAL::QueueCapability::Transfer
        );
    }
    
    GAL::QueueFamily::ID FindTransferQueueFamily() const noexcept {
        return FindQueueFamilyWithCapabilities(
            GAL::QueueCapability::Transfer
        );
    }

    GAL::Device get() noexcept { return m_device; }
};

class Instance {
    HInstance m_instance;
    std::vector<Device> devices;

public:
    Instance(HInstance instance);

    size_t GetDeviceCount() const { return devices.size(); }
    Device& GetDevice(size_t idx) { return devices[idx]; }

    GAL::Instance get() noexcept { return m_instance.get(); }
};
};
