#pragma once
#include "DeviceCommon.hpp"

#include <span>

namespace R1::GAL {
enum class ImageMemoryUsage {
    Default,
    RenderTarget,
    Swap,
};

namespace Detail {
template<typename Traits>
struct ImageConfigBase {
    Traits::ImageConfigFlags        flags;
    Traits::ImageType               type;    
    Traits::Format                  format;
    unsigned                        width;
    unsigned                        height;
    unsigned                        depth;
    unsigned                        mip_level_count;
    unsigned                        array_layer_count;
    unsigned                        sample_count;
    Traits::ImageUsageFlags         usage;
    std::span<const QueueFamilyID>  sharing_queue_families;
    Traits::ImageLayout             initial_layout;
    ImageMemoryUsage                memory_usage;
};

template<typename Traits>
struct ImageComponentMappingBase {
    Traits::ImageComponentSwizzle r, g, b, a;
};

template<typename Traits>
struct ImageSubresourceRangeBase {
    Traits::ImageAspectFlags    aspects;
    unsigned                    first_mip_level;
    unsigned                    mip_level_count;
    unsigned                    first_array_layer;
    unsigned                    array_layer_count;
};

template<typename Traits>
struct ImageViewConfigBase {
    Traits::ImageViewType               type;
    Traits::Format                      format;
    ImageComponentMappingBase<Traits>   components;
    ImageSubresourceRangeBase<Traits>   subresource_range;
};
}
}
