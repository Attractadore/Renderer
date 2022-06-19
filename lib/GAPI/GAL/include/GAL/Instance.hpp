#pragma once
#if GAL_USE_VULKAN
#include "VulkanDevice.hpp"
#include "VulkanInstance.hpp"
#endif

#include "Common/Flags.hpp"

#include <string>
#include <vector>

namespace R1::GAL {
namespace Detail {
template<typename E>
concept IsQueueCapability = requires(E e) {
    E::Graphics;
    E::Compute;
    E::Transfer;
};

template<typename E>
concept IsDeviceType = requires(E e) {
    E::Unknown;
    E::IntegratedGPU;
    E::DiscreteGPU;
    E::VirtualGPU;
    E::CPU;
};

static_assert(IsQueueCapability<QueueCapability>);
static_assert(IsDeviceType<DeviceType>);
}

using QueueCapabilityFlags = Flags<QueueCapability>;

enum QueueFamilyID: unsigned { Unknown = static_cast<unsigned>(-1) };

struct QueueFamily {
    using ID = QueueFamilyID;
    ID                      id;
    QueueCapabilityFlags    capabilities;
    unsigned                count;
};

struct DeviceDescription {
    std::string                 name;
    DeviceType                  type;
    std::vector<QueueFamily>    queue_families;
    bool                        wsi: 1;
};

struct InstanceDescription {
    bool wsi: 1;
    bool xlib: 1;
    bool xcb: 1;
};

struct InstanceConfig {};

InstanceDescription GetInstanceDescription();
void DestroyInstance(Instance instance);

size_t GetDeviceCount(Instance instance);
Device GetDevice(Instance instance, size_t idx = 0);

const DeviceDescription& GetDeviceDescription(Device dev);
}
