#pragma once
#if GAL_USE_VULKAN
#include "VulkanFormat.hpp"
#endif

namespace R1::GAL {
namespace Detail {
template<typename E>
concept IsFormat = requires(E e) {
    E::RGB8_UNORM;
    E::RGB8_SRGB;
    E::RGBA8_UNORM;
    E::RGBA8_SRGB;
    E::BGR8_UNORM;
    E::BGR8_SRGB;
    E::BGRA8_UNORM;
    E::BGRA8_SRGB;
    E::D32_FLOAT;
    E::Float;
    E::Float1;
    E::Float2;
    E::Float3;
    E::Float4;
};

static_assert(IsFormat<Format>);
}
}
