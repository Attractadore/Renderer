#pragma once
#include "Common/Interfaces.hpp"
#include "VKRAII.hpp"
#include "VKTypes.hpp"

#include <unordered_map>

namespace R1::VK {
struct PhysicalDevice {
    VkPhysicalDevice           device;
    VkPhysicalDeviceProperties device_properties;
    QueueFamilies              device_queue_families;
};

class Instance {
    Vk::Instance                                 m_instance;
    Vk::Surface                                  m_surface;
    std::unordered_map<DeviceID, PhysicalDevice> m_devices;

    void init();

public:
    Instance(::Display* dpy, ::Window win);
    Instance(xcb_connection_t*, xcb_window_t win);

    // This causes problems since by default move
    // happens in declaration order
    Instance& operator=(Instance&&) = delete;

    size_t deviceCount() const;
    size_t devices(std::span<DeviceID> out) const;
    const char* deviceName(DeviceID) const;

    Context* createContext(DeviceID dev);

private:
    bool DeviceIDIsValid(DeviceID dev) const;

    PhysicalDevice& getDevice(DeviceID dev);
    const PhysicalDevice& getDevice(DeviceID dev) const;
};
static_assert(IInstance<Instance>);
}
