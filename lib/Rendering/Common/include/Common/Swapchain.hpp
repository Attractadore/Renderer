#pragma once
#include "Image.hpp"

#include <functional>

namespace R1 {
using SurfaceSizeCallback = std::function<std::tuple<unsigned, unsigned>()>;

enum class ColorSpace;
enum class PresentMode;
enum class CompositeAlpha;
enum class Transform;

struct SurfaceFormat {
    Format format;
    ColorSpace color_space;
};

struct SurfaceDescription {
    std::vector<Transform>      supported_transforms;
    std::vector<CompositeAlpha> supported_composite_alphas;
    std::vector<SurfaceFormat>  supported_formats;
    std::vector<PresentMode>    supported_present_modes;
    unsigned                    min_image_count;
    unsigned                    max_image_count;
    ImageUsage                  supported_image_usage;
};

struct SwapchainCapabilities {
    bool image_mutable_format: 1;
    bool clipped: 1;
};

template<class Queue>
struct SwapchainConfigBase {
    SwapchainCapabilities               capabilities;
    ImageUsage                          image_usage;
    Format                              format;
    ColorSpace                          color_space;
    unsigned                            image_count;
    std::span<const QueueFamily::ID>    image_sharing_queue_families;
    Transform                           transform;
    CompositeAlpha                      composite_alpha;
    PresentMode                         present_mode;
    Queue                               present_queue;
};

template<class Surface>
struct SwapchainDescriptionBase {
    Surface                             surface;
    SwapchainCapabilities               capabilities;
    ImageUsage                          image_usage;
    Format                              format;
    ColorSpace                          color_space;
    unsigned                            image_count;
    unsigned                            width;
    unsigned                            height;
    std::vector<QueueFamily::ID>        image_sharing_queue_families;
    Transform                           transform;
    CompositeAlpha                      composite_alpha;
    PresentMode                         present_mode;
};
}