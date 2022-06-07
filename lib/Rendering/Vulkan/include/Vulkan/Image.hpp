#pragma once
#include "Common/Image.hpp"
#include "Context.hpp"
#include "Format.hpp"

namespace R1 {
enum class ImageType {
    D1 = VK_IMAGE_TYPE_1D,
    D2 = VK_IMAGE_TYPE_2D,
    D3 = VK_IMAGE_TYPE_3D,
};

enum class ImageLayout {
    Undefined       = VK_IMAGE_LAYOUT_UNDEFINED,
    General         = VK_IMAGE_LAYOUT_GENERAL,
    TransferSrc     = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
    TransferDst     = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
    Preinitilized   = VK_IMAGE_LAYOUT_PREINITIALIZED,
    ReadOnly        = VK_IMAGE_LAYOUT_READ_ONLY_OPTIMAL,
    Attachment      = VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL,
    Present         = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
};

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
}

namespace R1::VK {
struct ImageImpl;
using Image = ImageImpl*;
using ImageView = VkImageView;

Image CreateImage(Context ctx, const ImageConfig& config);
void DestroyImage(Context ctx, Image image);

ImageView CreateImageView(Context ctx, Image image, const ImageViewConfig& config);
void DestroyImageView(Context ctx, ImageView view);
}
