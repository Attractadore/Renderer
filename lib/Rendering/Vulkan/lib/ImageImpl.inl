#pragma once

namespace R1::VK {
constexpr inline VkImageCreateFlags ImageCapabilitiesToVK(
    const ImageCapabilities& caps
) {
    VkImageCreateFlags flags = 0;
    flags |= caps.mutable_format ? VK_IMAGE_CREATE_MUTABLE_FORMAT_BIT: 0;
    flags |= caps.cube_compatible ? VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT: 0;
    flags |= caps.block_texel_view_compatible ? VK_IMAGE_CREATE_BLOCK_TEXEL_VIEW_COMPATIBLE_BIT: 0;
    flags |= caps.extended_usage ? VK_IMAGE_CREATE_EXTENDED_USAGE_BIT: 0;
    return flags;
}

constexpr inline VkImageUsageFlags ImageUsageToVK(
    const ImageUsage& usg
) {
    VkImageUsageFlags flags = 0;
    flags |= usg.transfer_src ? VK_IMAGE_USAGE_TRANSFER_SRC_BIT: 0;
    flags |= usg.transfer_dst ? VK_IMAGE_USAGE_TRANSFER_DST_BIT: 0;
    flags |= usg.sampled ? VK_IMAGE_USAGE_SAMPLED_BIT: 0;
    flags |= usg.storage ? VK_IMAGE_USAGE_STORAGE_BIT: 0;
    flags |= usg.color_attachment ? VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT: 0;
    flags |= usg.depth_attachment ? VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT: 0;
    flags |= usg.stencil_attachment ? VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT: 0;
    flags |= usg.input_attachment ? VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT: 0;
    return flags;
}

constexpr ImageUsage ImageUsageFromVK(VkImageUsageFlags flags) {
    ImageUsage usg;
    usg.transfer_src = flags & VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
    usg.transfer_dst = flags & VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    usg.sampled = flags & VK_IMAGE_USAGE_SAMPLED_BIT;
    usg.storage = flags & VK_IMAGE_USAGE_STORAGE_BIT;
    usg.color_attachment = flags & VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    usg.depth_attachment = flags & VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
    usg.stencil_attachment = flags & VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
    usg.input_attachment = flags & VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT;
    return usg;
}

constexpr inline VkImageAspectFlags ImageAspectsToVK(
    const ImageAspects& aspects
) {
    VkImageAspectFlags flags = 0;
    flags |= aspects.color ? VK_IMAGE_ASPECT_COLOR_BIT: 0;
    flags |= aspects.depth ? VK_IMAGE_ASPECT_DEPTH_BIT: 0;
    flags |= aspects.stencil ? VK_IMAGE_ASPECT_STENCIL_BIT: 0;
    return flags;
}

constexpr inline VkImageSubresourceRange ImageSubresourceRangeToVK(
    const ImageSubresourceRange& range
) {
    return VkImageSubresourceRange {
        .aspectMask = ImageAspectsToVK(range.aspects),
        .baseMipLevel = range.first_mip_level,
        .levelCount = range.mip_level_count,
        .baseArrayLayer = range.first_array_layer,
        .layerCount = range.array_layer_count,
    };
}
}
