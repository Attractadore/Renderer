#pragma once
#include "ImageCommon.hpp"
#include "Traits.hpp"

namespace R1::GAL {
using ImageConfig = Detail::ImageConfigBase<Detail::Traits>;
using ImageComponentMapping = Detail::ImageComponentMappingBase<Detail::Traits>;
using ImageSubresourceRange = Detail::ImageSubresourceRangeBase<Detail::Traits>;
using ImageViewConfig = Detail::ImageViewConfigBase<Detail::Traits>;

Image CreateImage(Context ctx, const ImageConfig& config);
void DestroyImage(Context ctx, Image image);

ImageView CreateImageView(Context ctx, Image image, const ImageViewConfig& config);
void DestroyImageView(Context ctx, ImageView view);
}
