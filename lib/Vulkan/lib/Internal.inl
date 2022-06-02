#pragma once
#include "Internal.decl.hpp"
#include "Common/Declarations.hpp"

#include <span>

namespace R1::VK {
namespace Detail {
inline std::span<const char* const> GetRequiredInstanceLayers() {
    static constexpr auto layers = [] {
        if constexpr (R1::Debug) {
            return std::array {
                "VK_LAYER_KHRONOS_validation"
            };
        } else {
            return std::array<const char*, 0>{};
        }
    } ();
    return layers;
}

inline std::span<const char* const> GetRequiredInstanceExtensions() {
    static constexpr std::array<const char*, 0> exts;
    return exts;
}

Vk::Instance CreateVkInstance(
    const InstanceConfig& config,
    std::span<const char* const> layers,
    std::span<const char* const> exts
);
}

template<std::ranges::input_range R>
    requires std::same_as<const char*, std::ranges::range_value_t<R>>
Vk::Instance CreateVkInstanceWithExtensions(const InstanceConfig& config, R&& user_exts) {
    auto layers = Detail::GetRequiredInstanceLayers();

    auto req_exts = Detail::GetRequiredInstanceExtensions();
    std::vector exts(req_exts.begin(), req_exts.end());
    exts.insert(exts.end(), user_exts.begin(), user_exts.end());

    return Detail::CreateVkInstance(config, layers, exts);
}

inline QueueCapabilities QueueCapabilitiesFromVK(VkQueueFlags flags) {
    return {
        .graphics = (flags & VK_QUEUE_GRAPHICS_BIT) != 0,
        .compute = (flags & VK_QUEUE_COMPUTE_BIT) != 0,
        .transfer = (flags & VK_QUEUE_TRANSFER_BIT) != 0,
    };
}
}
