#pragma once
#include "ImageCommon.hpp"
#include "VulkanFormat.hpp"

#include <vulkan/vulkan.h>

namespace R1::GAL {
enum class ImageConfigOption {
    MutableFormat = VK_IMAGE_CREATE_MUTABLE_FORMAT_BIT,
    CubeCompatible = VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT,
    BlockTexelViewCompatible = VK_IMAGE_CREATE_BLOCK_TEXEL_VIEW_COMPATIBLE_BIT,
    ExtendedUsage = VK_IMAGE_CREATE_EXTENDED_USAGE_BIT,
};
using ImageConfigFlags = Flags<ImageConfigOption>;

enum class ImageType {
    D1 = VK_IMAGE_TYPE_1D,
    D2 = VK_IMAGE_TYPE_2D,
    D3 = VK_IMAGE_TYPE_3D,
};

enum class ImageUsage {
    TransferSRC = VK_IMAGE_USAGE_TRANSFER_SRC_BIT,
    TransferDST = VK_IMAGE_USAGE_TRANSFER_DST_BIT,
    Sampled = VK_IMAGE_USAGE_SAMPLED_BIT,
    Storage = VK_IMAGE_USAGE_STORAGE_BIT,
    ColorAttachment = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
    DepthAttachment = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
    StencilAttachment = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
};
using ImageUsageFlags = Flags<ImageUsage>;

enum class ImageLayout {
    Undefined       = VK_IMAGE_LAYOUT_UNDEFINED,
    General         = VK_IMAGE_LAYOUT_GENERAL,
    TransferSRC     = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
    TransferDST     = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
    Preinitilized   = VK_IMAGE_LAYOUT_PREINITIALIZED,
    ReadOnly        = VK_IMAGE_LAYOUT_READ_ONLY_OPTIMAL,
    Attachment      = VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL,
    Present         = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
};

enum class ImageAspect {
    Color = VK_IMAGE_ASPECT_COLOR_BIT,
    Depth = VK_IMAGE_ASPECT_DEPTH_BIT,
    Stencil = VK_IMAGE_ASPECT_STENCIL_BIT,
};
using ImageAspectFlags = Flags<ImageAspect>;

enum class ImageViewType {
    D1          = VK_IMAGE_VIEW_TYPE_1D,
    D2          = VK_IMAGE_VIEW_TYPE_2D,
    D3          = VK_IMAGE_VIEW_TYPE_3D,
    Cube        = VK_IMAGE_VIEW_TYPE_CUBE,
    D1Array     = VK_IMAGE_VIEW_TYPE_1D_ARRAY,
    D2Array     = VK_IMAGE_VIEW_TYPE_2D_ARRAY,
    CubeArray   = VK_IMAGE_VIEW_TYPE_CUBE_ARRAY,
};

enum class ImageComponentSwizzle {
    Identity    = VK_COMPONENT_SWIZZLE_IDENTITY,
    Zero        = VK_COMPONENT_SWIZZLE_ZERO,
    One         = VK_COMPONENT_SWIZZLE_ONE,
    R           = VK_COMPONENT_SWIZZLE_R,
    G           = VK_COMPONENT_SWIZZLE_G,
    B           = VK_COMPONENT_SWIZZLE_B,
    A           = VK_COMPONENT_SWIZZLE_A,
};

struct ImageImpl;
using Image = ImageImpl*;
using ImageView = VkImageView;

namespace Detail {
struct ImageTraits {
    using Format = GAL::Format;
    using ImageAspect = GAL::ImageAspect;
    using ImageComponentSwizzle = GAL::ImageComponentSwizzle;
    using ImageConfigOption = GAL::ImageConfigOption;
    using ImageLayout = GAL::ImageLayout;
    using ImageType = GAL::ImageType;
    using ImageUsage = GAL::ImageUsage;
    using ImageViewType = GAL::ImageViewType;
};
}

using ImageConfig = Detail::ImageConfigBase<Detail::ImageTraits>;
using ImageComponentMapping = Detail::ImageComponentMappingBase<Detail::ImageTraits>;
using ImageSubresourceRange = Detail::ImageSubresourceRangeBase<Detail::ImageTraits>;
using ImageViewConfig = Detail::ImageViewConfigBase<Detail::ImageTraits>;
}
