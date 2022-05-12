#pragma once
#include <array>
#include <memory>

namespace R1 {
#if 0
// CTAD doesn't work for unique_ptr since T* can represent
// either a single element or an array
template<typename T, typename Deleter = std::default_delete<T>>
class single_unique_ptr: public std::unique_ptr<T, Deleter> {
public:
    explicit single_unique_ptr(T* p, Deleter&& d): std::unique_ptr<T, Deleter>(p, std::move(d)) {}
};

template<typename T, typename Deleter = std::default_delete<T>>
explicit single_unique_ptr(T*, Deleter&& d) -> single_unique_ptr<T, Deleter>;
#endif

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
