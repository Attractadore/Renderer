#pragma once
#include <array>

namespace R1 {
template<typename T, typename... Us>
constexpr auto make_array(Us&&... args) {
    return std::array<T, sizeof...(Us)> {
        std::forward<Us>(args)...
    };
}
}
