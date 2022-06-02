#pragma once
#include "Types.hpp"

namespace R1::VK {
template<std::ranges::input_range R>
    requires std::same_as<const char*, std::ranges::range_value_t<R>>
Vk::Instance CreateVkInstanceWithExtensions(const InstanceConfig& config, R&& exts);
Instance CreateInstanceFromVK(Vk::Instance vk_instance);

QueueCapabilities QueueCapabilitiesFromVK(VkQueueFlags flags);
}
