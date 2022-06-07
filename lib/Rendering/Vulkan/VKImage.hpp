#pragma once
#include "Common/Ref.hpp"
#include "VKFormat.hpp"
#include "VKRAII.hpp"

#include <map>
#include <optional>
#include <variant>

namespace R1::VK {
struct Image;
using ImageRef = Ref<Image>;

enum class ImageViewType: uint8_t {
    D2 = VK_IMAGE_VIEW_TYPE_2D,
};

enum class ImageType: uint8_t {
    D2 = VK_IMAGE_TYPE_2D,
};

enum class ImageTiling: uint8_t {
    Linear = VK_IMAGE_TILING_LINEAR,
    Optimal = VK_IMAGE_TILING_OPTIMAL,
};

struct ImageUsage {
    bool color_attachment: 1;
};

constexpr inline VkImageUsageFlags ImageUsageToFlags(const ImageUsage& usg) {
   return usg.color_attachment ? VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT: 0; 
}

constexpr inline ImageUsage ImageUsageFromFlags(VkImageUsageFlags usg_flgs) {
    return {
        .color_attachment = bool(usg_flgs & VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT),
    };
}

namespace Detail {
struct MipLevels {
    unsigned first = 0;
    unsigned count = 1;

    auto operator<=>(const MipLevels&) const = default;
};

struct ArrayLayers {
    unsigned                first = 0;
    std::optional<unsigned> count;

    auto operator<=>(const ArrayLayers&) const = default;
};

struct Cubes {
    unsigned                first_layer = 0;
    std::optional<unsigned> count;

    auto operator<=>(const Cubes&) const = default;
};

struct ImageViewConfig {
    using MipLevels = Detail::MipLevels;
    using ArrayLayers = Detail::ArrayLayers;
    using Cubes = Detail::Cubes;

    struct D1SubresourceRange {
        MipLevels   mip_levels;
        ArrayLayers array_layers;

        auto operator<=>(const D1SubresourceRange&) const = default;
    };

    struct D2SubresourceRange {
        MipLevels   mip_levels;
        ArrayLayers array_layers;

        auto operator<=>(const D2SubresourceRange&) const = default;
    };

    struct D3SubresourceRange {
        MipLevels   mip_levels;
        unsigned    first_array_layer = 0;

        auto operator<=>(const D3SubresourceRange&) const = default;
    };

    struct CubeSubresourceRange {
        MipLevels   mip_levels;
        Cubes       cubes;

        auto operator<=>(const CubeSubresourceRange&) const = default;
    };

    std::variant<
        D1SubresourceRange, 
        D2SubresourceRange,
        D3SubresourceRange,
        CubeSubresourceRange
    > subresource_range;

    auto operator<=>(const ImageViewConfig&) const = default;
};

struct ImageViewDescription {
    const Image&    image;
    ImageViewType   type;
    struct {
        MipLevels mip_levels;
        ArrayLayers array_layers;
    } subresource_range;
};

struct ImageView {
    using Type = ImageViewType;
    using Config = ImageViewConfig;
    using Description = ImageViewDescription;

    Vk::ImageView   m_view;

private:
    Description     m_description;

public:
    ImageView(Vk::ImageView view, const Description& desc):
        m_view{std::move(view)},
        m_description{desc} {}

    const auto& GetDescription() const {
        return m_description;
    }
};

struct ImageConfig {};

struct ImageDescription {
    ImageType   type;
    ImageFormat format;
    unsigned    width;
    unsigned    height;
    unsigned    depth;
    unsigned    mip_level_count;
    unsigned    array_layer_count;
    unsigned    sample_count;
    ImageTiling tiling;
    ImageUsage  usage;
};
}

struct Image: RefedBase<Image> {
    using Type = ImageType;
    using Format = ImageFormat;
    using Tiling = ImageTiling;
    using Usage = ImageUsage;

    using View = Detail::ImageView;
    using Config = Detail::ImageConfig;
    using Description = Detail::ImageDescription;

    Vk::Image                       m_image;

private:
    Description                     m_description;
    std::map<View::Config, View>    m_views;

protected:
    Image(Vk::Image image, const Description& description):
        m_image{std::move(image)},
        m_description{description} {}

public:
    const View& GetView(const View::Config& config);
    const auto& GetDescription() const {
        return m_description;
    }
};
}
