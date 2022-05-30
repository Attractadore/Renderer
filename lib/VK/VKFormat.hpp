#pragma once
#include <vulkan/vulkan.h>

namespace R1::VK {
enum class VertexFormat {
    Float   = VK_FORMAT_R32_SFLOAT,
    Float2  = VK_FORMAT_R32G32_SFLOAT,
    Float3  = VK_FORMAT_R32G32B32_SFLOAT,
    Float4  = VK_FORMAT_R32G32B32A32_SFLOAT,
};

enum class ColorFormat {
    RGB8_UNORM  = VK_FORMAT_R8G8B8_UNORM,
    RGBA8_UNORM = VK_FORMAT_R8G8B8A8_UNORM,
    BGR8_UNORM  = VK_FORMAT_B8G8R8_UNORM,
    BGRA8_UNORM = VK_FORMAT_B8G8R8A8_UNORM,
};

enum class DepthStencilFormat {};

constexpr inline bool IsDepthOnlyFormat(DepthStencilFormat fmt) {
    using enum DepthStencilFormat;
    switch(fmt) {
        default:
            return false;
    }
}

constexpr inline bool IsStencilOnlyFormat(DepthStencilFormat fmt) {
    using enum DepthStencilFormat;
    switch(fmt) {
        default:
            return false;
    }
}

constexpr inline bool IsDepthStencilFormat(DepthStencilFormat fmt) {
    return not (IsDepthOnlyFormat(fmt) or IsStencilOnlyFormat(fmt));
}
}
