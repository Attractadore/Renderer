#pragma once
#include "ImageCommon.hpp"

#include <functional>

namespace R1::GAL {
using SurfaceSizeCallback = std::function<std::tuple<unsigned, unsigned>()>;

namespace Detail {
template<typename Traits>
struct SurfaceFormatBase {
    Traits::Format      format;
    Traits::ColorSpace  color_space;
};

template<typename Traits>
struct SurfaceDescriptionBase {
    std::vector<SurfaceFormatBase<Traits>>          supported_formats;
    std::vector<typename Traits::PresentMode>       supported_present_modes;
    std::vector<typename Traits::CompositeAlpha>    supported_composite_alphas;
    unsigned                                        min_image_count;
    unsigned                                        max_image_count;
    Traits::ImageUsageFlags                         supported_image_usage;
};

template<typename Traits>
struct SwapchainConfigBase {
    Traits::Format                  format;
    Traits::ColorSpace              color_space;
    Traits::ImageUsageFlags         image_usage;
    unsigned                        image_count;
    std::span<const QueueFamilyID>  image_sharing_queue_families;
    Traits::CompositeAlpha          composite_alpha;
    Traits::PresentMode             present_mode;
    Traits::Queue                   present_queue;
    bool                                clipped: 1;
};
}

enum class SwapchainStatus {
    Good,
    RequiresFastResize,
    RequiresSlowResize,
};
}
