#pragma once
#include "GAL/GAL.hpp"

#include <cassert>

namespace R1::GAPI {
namespace Detail {
struct FormatTraits {
    bool is_color: 1 = false;
    bool is_srgb: 1 = false;
};

constexpr inline FormatTraits GetFormatTraits(GAL::Format fmt) {
    using enum GAL::Format;
    switch (fmt) {
        case RGB8_UNORM:
        case RGBA8_UNORM:
        case BGR8_UNORM:
        case BGRA8_UNORM:
            return {
                .is_color = true,
            };
        case RGB8_SRGB:
        case RGBA8_SRGB:
        case BGR8_SRGB:
        case BGRA8_SRGB:
            return {
                .is_color = true,
                .is_srgb = true,
            };
        default:
            return {};
    }
}
}

constexpr inline bool IsColorFormat(GAL::Format fmt) {
    return Detail::GetFormatTraits(fmt).is_color;
}

constexpr inline bool IsSRGBFormat(GAL::Format fmt) {
    return Detail::GetFormatTraits(fmt).is_srgb;
}
}
