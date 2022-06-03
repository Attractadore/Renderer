#pragma once
#include "Instance.hpp"

#include <span>

namespace R1::VK {
Vk::Instance CreateVkInstanceWithExtensions(
    const InstanceConfig& config, std::span<const char* const> exts
);
Instance CreateInstanceFromVK(Vk::Instance vk_instance);
}
