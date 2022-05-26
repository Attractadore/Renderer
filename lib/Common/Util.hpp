#pragma once
#include <array>
#include <memory>

namespace R1 {
template<typename T, typename... Us>
constexpr auto make_array(Us&&... args) {
    return std::array<T, sizeof...(Us)> {
        std::forward<Us>(args)...
    };
}

template<typename Tp, typename Deleter>
using Handle = std::unique_ptr<
    std::remove_pointer_t<Tp>, Deleter
>;
}
