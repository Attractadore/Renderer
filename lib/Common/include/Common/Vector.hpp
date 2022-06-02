#pragma once
#include <algorithm>
#include <ranges>
#include <vector>

namespace R1 {
template<std::ranges::input_range R>
constexpr auto vec_from_range(R&& r) {
    using T = std::ranges::range_value_t<R>;
    std::vector<T> vec;
    if constexpr (std::ranges::sized_range<R>) {
        vec.reserve(std::ranges::size(r));
    }
    vec.assign(r.begin(), r.end());
    return vec;
}
}
