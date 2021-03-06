#pragma once
#include <concepts>

namespace R1 {
namespace Detail {
template<typename E>
concept ScopedEnum = not std::convertible_to<E, std::underlying_type_t<E>>;

enum class TestEnumClass {};
static_assert(ScopedEnum<TestEnumClass>);
enum TestEnum {};
static_assert(not ScopedEnum<TestEnum>);

struct EmptyFlagsT {};
}
inline constexpr Detail::EmptyFlagsT EmptyFlags;

template<Detail::ScopedEnum E>
class Flags {
    E value = E(0);

public:
    using BitType = E;
    using UnderlyingType = std::underlying_type_t<E>;

    constexpr Flags() noexcept = default;
    constexpr Flags(decltype(EmptyFlags)) noexcept {}
    constexpr Flags(BitType bit) noexcept:
        value{bit} {}

    constexpr BitType operator&(BitType r) const noexcept {
        return static_cast<E>(
            static_cast<UnderlyingType>(value) & static_cast<UnderlyingType>(r)
        );
    }

    constexpr Flags<E> operator&(Flags<E> r) const noexcept {
        using U = std::underlying_type_t<E>;
        return static_cast<E>(
            static_cast<U>(value) & static_cast<U>(r.value)
        );
    }

    constexpr Flags<E>& operator&=(Flags<E> r) noexcept {
        return *this = *this & r;
    }

    constexpr Flags<E> operator|(Flags<E> r) const noexcept {
        using U = std::underlying_type_t<E>;
        return static_cast<E>(
            static_cast<U>(value) | static_cast<U>(r.value)
        );
    }

    constexpr Flags<E>& operator|=(Flags<E> r) noexcept {
        return *this = *this | r;
    }

    constexpr bool IsSet(E flag) const noexcept {
        return static_cast<UnderlyingType>(*this & flag);
    }

    constexpr bool AnySet(Flags<E> mask) const noexcept {
        return (*this & mask) != EmptyFlags;
    }

    constexpr bool AllSet(Flags<E> mask) const noexcept {
        return (*this & mask) == mask;
    }

    constexpr bool NoneSet(Flags<E> mask) const noexcept {
        return (*this & mask) == EmptyFlags;
    }

    constexpr E Extract() const noexcept {
        return value;
    }

    constexpr explicit operator bool() const noexcept {
        return static_cast<UnderlyingType>(value);
    }

    constexpr bool operator==(const Flags<E>&) const noexcept = default;
};

template<Detail::ScopedEnum E>
constexpr E operator&(E l, E r) {
    using U = std::underlying_type_t<E>;
    return static_cast<E>(
        static_cast<U>(l) & static_cast<U>(r)
    );
}

template<Detail::ScopedEnum E>
constexpr E operator&(E l, Flags<E> r) {
    return r & l;
}

template<Detail::ScopedEnum E>
constexpr Flags<E> operator|(E l, E r) {
    using U = std::underlying_type_t<E>;
    return static_cast<E>(
        static_cast<U>(l) | static_cast<U>(r)
    );
}

template<Detail::ScopedEnum E>
constexpr Flags<E> operator|(E l, Flags<E> r) {
    return r | l;
}
}
