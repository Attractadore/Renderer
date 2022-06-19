#pragma once
#if GAL_USE_VULKAN
#include "VulkanImage.hpp"
#endif

#include "Common/Flags.hpp"
#include "Context.hpp"

namespace R1::GAL {
namespace Detail {
template<typename E>
concept IsImageConfigOption = requires(E e) {
    E::MutableFormat;
    E::CubeCompatible;
    E::BlockTexelViewCompatible;
    E::ExtendedUsage;
};

template<typename E>
concept IsImageType = requires(E e) {
    E::D1;
    E::D2;
    E::D3;
};

template<typename E>
concept IsImageUsage = requires(E e) {
    E::TransferSRC;
    E::TransferDST;
    E::Sampled;
    E::Storage;
    E::ColorAttachment;
    E::DepthAttachment;
    E::StencilAttachment;
};

template<typename E>
concept IsImageLayout = requires(E e) {
    E::Undefined;
    E::General;
    E::TransferSRC;
    E::TransferDST;
    E::Preinitilized;
    E::ReadOnly;
    E::Attachment;
    E::Present;
};

template<typename E>
concept IsImageAspect = requires(E e) {
    E::Color;
    E::Depth;
    E::Stencil;
};

template<typename E>
concept IsImageViewType = requires(E e) {
    E::D1;
    E::D2;
    E::D3;
    E::Cube;
    E::D1Array;
    E::D2Array;
    E::CubeArray;
};

template<typename E>
concept IsImageComponentSwizzle = requires(E e) {
    E::Identity;
    E::Zero;
    E::One;
    E::R;
    E::G;
    E::B;
    E::A;
};

static_assert(IsImageConfigOption<ImageConfigOption>);
static_assert(IsImageType<ImageType>);
static_assert(IsImageUsage<ImageUsage>);
static_assert(IsImageAspect<ImageAspect>);
static_assert(IsImageViewType<ImageViewType>);
static_assert(IsImageComponentSwizzle<ImageComponentSwizzle>);
};

using ImageConfigFlags  = Flags<ImageConfigOption>;
using ImageUsageFlags   = Flags<ImageUsage>;
using ImageAspectFlags  = Flags<ImageAspect>;

enum class ImageMemoryUsage {
    Default,
    Dedicated,
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
