#pragma once
#include <vector>

#include <range/v3/all.hpp>

namespace R1 {
template<ranges::input_range R>
constexpr auto VecFromRange(R&& r) {
    auto v = ranges::views::common(r);
    using T = ranges::range_value_t<R>;
    std::vector<T> vec;
    if constexpr (ranges::sized_range<R>) {
        vec.reserve(ranges::size(r));
    }
    vec.assign(v.begin(), v.end());
    return vec;
}

template<ranges::input_range R>
constexpr auto vec_from_range(R&& r) {
    return VecFromRange(std::forward<R>(r));
}

template<typename Vector, ranges::input_range R>
constexpr auto VecAppend(Vector& vec, R&& r) {
    auto v = ranges::views::common(r);
    return vec.insert(vec.end(), v.begin(), v.end());
}
}
