#pragma once
#if GAL_USE_VULKAN
#include "VulkanImage.hpp"
#endif

#include "Common/Flags.hpp"
#include "Context.hpp"

namespace R1::GAL {
using ImageConfigFlags  = Flags<ImageConfigOption>;
using ImageUsageFlags   = Flags<ImageUsage>;
using ImageAspectFlags  = Flags<ImageAspect>;

enum class ImageMemoryUsage {
    Default,
    RenderTarget,
    Swap,
};

struct ImageConfig {
    ImageConfigFlags                    flags;
    ImageType                           type;    
    Format                              format;
    unsigned                            width;
    unsigned                            height;
    unsigned                            depth;
    unsigned                            mip_level_count;
    unsigned                            array_layer_count;
    unsigned                            sample_count;
    ImageUsageFlags                     usage;
    std::span<const QueueFamily::ID>    sharing_queue_families;
    ImageLayout                         initial_layout;
    ImageMemoryUsage                    memory_usage;
};

struct ImageComponentMapping {
    ImageComponentSwizzle r, g, b, a;
};

struct ImageSubresourceRange {
    ImageAspectFlags    aspects;
    unsigned            first_mip_level;
    unsigned            mip_level_count;
    unsigned            first_array_layer;
    unsigned            array_layer_count;
};

struct ImageViewConfig {
    ImageViewType               type;
    Format                      format;
    ImageComponentMapping       components;
    ImageSubresourceRange       subresource_range;
};

Image CreateImage(Context ctx, const ImageConfig& config);
void DestroyImage(Context ctx, Image image);

ImageView CreateImageView(Context ctx, Image image, const ImageViewConfig& config);
void DestroyImageView(Context ctx, ImageView view);
}
