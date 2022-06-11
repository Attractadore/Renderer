#pragma once
#include "Image.hpp"

#include <functional>

namespace R1 {
using SurfaceSizeCallback = std::function<std::tuple<unsigned, unsigned>()>;

enum class ColorSpace;
enum class PresentMode;
enum class CompositeAlpha;

struct SurfaceFormat {
    Format format;
    ColorSpace color_space;
};

struct SurfaceDescription {
    std::vector<SurfaceFormat>  supported_formats;
    std::vector<PresentMode>    supported_present_modes;
    std::vector<CompositeAlpha> supported_composite_alphas;
    unsigned                    min_image_count;
    unsigned                    max_image_count;
    ImageUsage                  supported_image_usage;
};

template<class Queue>
struct SwapchainConfigBase {
    Format                              format;
    ColorSpace                          color_space;
    unsigned                            image_count;
    std::span<const QueueFamily::ID>    image_sharing_queue_families;
    CompositeAlpha                      composite_alpha;
    PresentMode                         present_mode;
    Queue                               present_queue;
    ImageUsage                          image_usage;
    bool                                clipped: 1;
};

enum class SwapchainStatus {
    Good,
    RequiresFastResize,
    RequiresSlowResize,
};
}
