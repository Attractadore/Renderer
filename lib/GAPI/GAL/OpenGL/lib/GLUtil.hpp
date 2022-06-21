#pragma once
#include <stdexcept>

namespace R1::GAL {
inline void ThrowIfFailed(bool good, const char* src) {
    if (!good) {
        throw std::runtime_error{src};
    }
}
}
