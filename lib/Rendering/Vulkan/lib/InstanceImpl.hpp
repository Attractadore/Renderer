#pragma once
#include "Instance.hpp"
#include "DeviceImpl.hpp"
#include "VKRAII.hpp"

#include <span>

namespace R1::VK {
struct InstanceImpl {
    Vk::Instance instance;
    std::vector<DeviceImpl> devices;
};

Vk::Instance CreateVkInstanceWithExtensions(
    const InstanceConfig& config, std::span<const char* const> exts
);
Instance CreateInstanceFromVK(Vk::Instance vk_instance);
}
