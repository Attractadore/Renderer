#pragma once
#include "Device.hpp"
#include "Format.hpp"

#include <span>

namespace R1 {
struct ImageCapabilities {
    bool mutable_format: 1;
    bool cube_compatible: 1;
    bool block_texel_view_compatible: 1;
    bool extended_usage: 1;
};

enum class ImageType;

struct ImageUsage {
    bool transfer_src: 1;
    bool transfer_dst: 1;
    bool sampled: 1;
    bool storage: 1;
    bool color_attachment: 1;
    bool depth_attachment: 1;
    bool stencil_attachment: 1;
    bool input_attachment: 1;
};

enum class ImageLayout;
enum class ImageMemoryUsage {
    Default,
    RenderTarget,
    Swap,
};

struct ImageConfig {
    ImageCapabilities                   capabilities;
    ImageType                           type;    
    Format                              format;
    unsigned                            width;
    unsigned                            height;
    unsigned                            depth;
    unsigned                            mip_level_count;
    unsigned                            array_layer_count;
    unsigned                            sample_count;
    ImageUsage                          usage;
    std::span<const QueueFamily::ID>    sharing_queue_families;
    ImageLayout                         initial_layout;
    ImageMemoryUsage                    memory_usage;
};

struct ImageAspects {
    bool color: 1;
    bool depth: 1;
    bool stencil: 1;
};

enum class ImageViewType;
enum class ImageComponentSwizzle;

struct ImageComponentMapping {
    ImageComponentSwizzle r, g, b, a;
};

struct ImageSubresourceRange {
    ImageAspects    aspects;
    unsigned        first_mip_level;
    unsigned        mip_level_count;
    unsigned        first_array_layer;
    unsigned        array_layer_count;
};

struct ImageViewConfig {
    ImageViewType           type;
    Format                  format;
    ImageComponentMapping   components;
    ImageSubresourceRange   subresource_range;
};
}
