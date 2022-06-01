#pragma once
#include <vulkan/vulkan.h>

#include <cassert>

namespace R1::VK {
enum class BufferFormat: uint8_t {
    Float   = VK_FORMAT_R32_SFLOAT,
    Float2  = VK_FORMAT_R32G32_SFLOAT,
    Float3  = VK_FORMAT_R32G32B32_SFLOAT,
    Float4  = VK_FORMAT_R32G32B32A32_SFLOAT,
};

enum class ImageFormat: uint8_t {
    RGB8_UNORM  = VK_FORMAT_R8G8B8_UNORM,
    RGB8_SRGB   = VK_FORMAT_R8G8B8_SRGB,
    RGBA8_UNORM = VK_FORMAT_R8G8B8A8_UNORM,
    RGBA8_SRGB  = VK_FORMAT_R8G8B8A8_SRGB,
    BGR8_UNORM  = VK_FORMAT_B8G8R8_UNORM,
    BGR8_SRGB   = VK_FORMAT_B8G8R8_SRGB,
    BGRA8_UNORM = VK_FORMAT_B8G8R8A8_UNORM,
    BGRA8_SRGB  = VK_FORMAT_B8G8R8A8_SRGB,
};

namespace Detail {
struct ImageFormatTraits {
    bool is_color: 1 = false;
    bool is_srgb: 1 = false;
    bool is_depth: 1 = false;
    bool is_stencil: 1 = false;
};

constexpr inline ImageFormatTraits GetImageFormatTraits(ImageFormat fmt) {
    using enum ImageFormat;
    switch (fmt) {
        case RGB8_UNORM:
        case RGBA8_UNORM:
        case BGR8_UNORM:
        case BGRA8_UNORM: {
            return {
                .is_color = true,
            };
        }
        case RGB8_SRGB:
        case RGBA8_SRGB:
        case BGR8_SRGB:
        case BGRA8_SRGB: {
            return {
                .is_color = true,
                .is_srgb = true,
            };
        }
        default:
            assert(!"Unknown format");
    }
}
}

constexpr inline bool IsColorFormat(ImageFormat fmt) {
    return Detail::GetImageFormatTraits(fmt).is_color;
}

constexpr inline bool IsSRGBFormat(ImageFormat fmt) {
    return Detail::GetImageFormatTraits(fmt).is_srgb;
}

constexpr inline bool IsDepthFormat(ImageFormat fmt) {
    return Detail::GetImageFormatTraits(fmt).is_depth;
}

constexpr inline bool IsStencilFormat(ImageFormat fmt) {
    return Detail::GetImageFormatTraits(fmt).is_stencil;
    return false;
}
}
