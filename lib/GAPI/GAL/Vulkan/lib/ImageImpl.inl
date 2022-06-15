#pragma once

namespace R1::GAL {
constexpr inline VkImageSubresourceRange ImageSubresourceRangeToVK(
    const ImageSubresourceRange& range
) {
    return VkImageSubresourceRange {
        .aspectMask = static_cast<VkImageAspectFlags>(range.aspects.Extract()),
        .baseMipLevel = range.first_mip_level,
        .levelCount = range.mip_level_count,
        .baseArrayLayer = range.first_array_layer,
        .layerCount = range.array_layer_count,
    };
}
}
