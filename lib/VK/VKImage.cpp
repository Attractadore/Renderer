#include "VKImage.hpp"

namespace R1::VK {
namespace {
std::tuple<
    VkImageView,
    Image::View::Description
> createImageView(const Image& img, const Image::View::Config& config) {
    auto fmt = img.GetDescription().format;

    VkImageViewCreateInfo create_info = {
        .sType = sType(create_info),
        .image = img.m_image.get(),
        .format = static_cast<VkFormat>(fmt),
    };

    Image::View::Description view_desc = {
        .image = img,
    };

    std::visit([&] <typename S> (const S& sr) {
        auto& vksr = create_info.subresourceRange;
        auto& vkvt = create_info.viewType;

        bool color_aspect = IsColorFormat(fmt);
        bool depth_aspect = IsDepthFormat(fmt);
        bool stencil_aspect = IsStencilFormat(fmt);
        if (color_aspect) {
            vksr.aspectMask |= VK_IMAGE_ASPECT_COLOR_BIT;
        }
        if (depth_aspect) {
            vksr.aspectMask |= VK_IMAGE_ASPECT_DEPTH_BIT;
        }
        if (stencil_aspect) {
            vksr.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
        }

        vksr.baseMipLevel = sr.mip_levels.first;
        vksr.levelCount = sr.mip_levels.count;

        using C = Image::View::Config;
        if constexpr (std::same_as<S, C::D1SubresourceRange>) {
            vksr.baseArrayLayer = sr.array_layers.first;
            if (!sr.array_layers.count) {
                vksr.layerCount = 1;
                vkvt = VK_IMAGE_VIEW_TYPE_1D;
            } else {
                vksr.layerCount = *sr.array_layers.count;
                vkvt = VK_IMAGE_VIEW_TYPE_1D_ARRAY;
            }
        } else if constexpr(std::same_as<S, C::D2SubresourceRange>) {
            vksr.baseArrayLayer = sr.array_layers.first;
            if (!sr.array_layers.count) {
                vksr.layerCount = 1;
                vkvt = VK_IMAGE_VIEW_TYPE_2D;
            } else {
                vksr.layerCount = *sr.array_layers.count;
                vkvt = VK_IMAGE_VIEW_TYPE_2D_ARRAY;
            }
        } else if constexpr(std::same_as<S, C::D3SubresourceRange>) {
            vksr.baseArrayLayer = sr.first_array_layer;
            vksr.layerCount = 1;
            vkvt = VK_IMAGE_VIEW_TYPE_3D;
        } else if constexpr(std::same_as<S, C::CubeSubresourceRange>) {
            vksr.baseArrayLayer = sr.cubes.first_layer;
            if (!sr.cubes.count) {
                vksr.layerCount = 6;
                vkvt = VK_IMAGE_VIEW_TYPE_CUBE;
            } else {
                vksr.layerCount = 6 * *sr.cubes.count;
                vkvt = VK_IMAGE_VIEW_TYPE_CUBE_ARRAY;
            }
        }

        view_desc.type = static_cast<Image::View::Type>(vkvt);
        view_desc.subresource_range = {
            .mip_levels = {
                .first = vksr.baseMipLevel,
                .count = vksr.levelCount,
            },
            .array_layers = {
                .first = vksr.baseArrayLayer,
                .count = vksr.layerCount,
            },
        };
    }, config.subresource_range);

    VkImageView view = VK_NULL_HANDLE;
    vkCreateImageView(img.m_image.get_device(), &create_info, nullptr, &view);

    return {view, view_desc};
}
}

const Image::View& Image::GetView(const Image::View::Config& config) {
    auto it = m_views.find(config);
    if (it == m_views.end()) {
        auto [view, view_desc] = createImageView(*this, config);
        if (!view) {
            throw std::runtime_error{"Vulkan: Failed to create image view"};
        }
        Vk::ImageView handle{m_image.get_device(), view};
        it = m_views.emplace(
            std::piecewise_construct,
            std::forward_as_tuple(config),
            std::forward_as_tuple(
                std::move(handle), 
                view_desc
            )
        ).first;
    }
    return it->second;
}
}
