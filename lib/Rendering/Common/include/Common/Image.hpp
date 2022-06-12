#pragma once
#include "Device.hpp"
#include "Format.hpp"

#include <span>

namespace R1 {
enum class ImageConfigOption;
using ImageConfigFlags = Flags<ImageConfigOption>;
enum class ImageType;
enum class ImageUsage;
using ImageUsageFlags = Flags<ImageUsage>;
enum class ImageLayout;

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

enum class ImageAspect;
using ImageAspectFlags = Flags<ImageAspect>;
enum class ImageViewType;
enum class ImageComponentSwizzle;

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
    ImageViewType           type;
    Format                  format;
    ImageComponentMapping   components;
    ImageSubresourceRange   subresource_range;
};
}
