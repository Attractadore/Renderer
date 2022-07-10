#pragma once
#include <vulkan/vulkan.h>

namespace R1::GAL {
enum class Format {
    RGB8_UNORM  = VK_FORMAT_R8G8B8_UNORM,
    RGB8_SRGB   = VK_FORMAT_R8G8B8_SRGB,
    RGBA8_UNORM = VK_FORMAT_R8G8B8A8_UNORM,
    RGBA8_SRGB  = VK_FORMAT_R8G8B8A8_SRGB,
    BGR8_UNORM  = VK_FORMAT_B8G8R8_UNORM,
    BGR8_SRGB   = VK_FORMAT_B8G8R8_SRGB,
    BGRA8_UNORM = VK_FORMAT_B8G8R8A8_UNORM,
    BGRA8_SRGB  = VK_FORMAT_B8G8R8A8_SRGB,

    Float       = VK_FORMAT_R32_SFLOAT,
    Float1      = Float,
    Float2      = VK_FORMAT_R32G32_SFLOAT,
    Float3      = VK_FORMAT_R32G32B32_SFLOAT,
    Float4      = VK_FORMAT_R32G32B32A32_SFLOAT,
};
}
