#pragma once
#include "GAPIRAII.hpp"

#include <memory>

namespace R1::Rendering {
class Device {
    GAPI::Device m_device;

public:
    Device(GAPI::Device device):
        m_device{device} {}

    const std::string& GetName() const;
    const DeviceDescription& GetDescription() const;

    GAPI::Device get() noexcept { return m_device; }
};

class Instance {
    HInstance m_instance;
    std::vector<Device> devices;

public:
    Instance(HInstance instance);

    size_t GetDeviceCount() const { return devices.size(); }
    Device& GetDevice(size_t idx) { return devices[idx]; }

    GAPI::Instance get() noexcept { return m_instance.get(); }
};
};
