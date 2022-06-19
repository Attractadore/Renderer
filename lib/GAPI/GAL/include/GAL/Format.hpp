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
};

static_assert(IsFormat<Format>);
}
}
