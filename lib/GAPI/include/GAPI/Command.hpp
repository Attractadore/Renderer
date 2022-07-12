#pragma once
#include "GAL/GAL.hpp"

#include <cassert>

namespace R1::GAPI {
inline constexpr size_t IndexFormatSize(GAL::IndexFormat fmt) noexcept {
    switch(fmt) {
        case GAL::IndexFormat::U16:
            return sizeof(uint16_t);
        case GAL::IndexFormat::U32:
            return sizeof(uint32_t);
        default:
            assert(!"Unknown index format");
    }
}
}
