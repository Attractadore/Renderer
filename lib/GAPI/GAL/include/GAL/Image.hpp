#pragma once
#include "Types.hpp"

namespace R1::GAL {
Image CreateImage(Context ctx, const ImageConfig& config);
void DestroyImage(Context ctx, Image image);

ImageView CreateImageView(Context ctx, Image image, const ImageViewConfig& config);
void DestroyImageView(Context ctx, ImageView view);
}
