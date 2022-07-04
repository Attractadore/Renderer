#pragma once
#include "GAPI/Instance.hpp"
#include "R1Types.h"
#include "Scene.hpp"

namespace R1 {
namespace Detail {
using PublicPrivateBij = std::tuple<
    R1Device*,      R1::GAPI::Device*,
    R1Mesh,         R1::MeshID,
    R1MeshInstance, R1::MeshInstanceID
>;

template<typename T>
struct TypeHolder {
    using type = T;
};

template<typename T, size_t I = 0, typename Bij = PublicPrivateBij>
    requires (I < std::tuple_size_v<Bij>)
constexpr auto GetPrivate() {
    using Pub = std::tuple_element_t<I, Bij>;
    using Pri = std::tuple_element_t<I + 1, Bij>;
    if constexpr (std::same_as<T, Pub>) {
        return TypeHolder<Pri>{};
    } else {
        return GetPrivate<T, I + 2, Bij>();
    }
}

template<typename T, size_t I = 0, typename Bij = PublicPrivateBij>
    requires (I < std::tuple_size_v<Bij>)
constexpr auto GetPublic() {
    using Pub = std::tuple_element_t<I, Bij>;
    using Pri = std::tuple_element_t<I + 1, Bij>;
    if constexpr (std::same_as<T, Pri>) {
        return TypeHolder<Pub>{};
    } else {
        return GetPublic<T, I + 2, Bij>();
    }
}

template<typename T>
using PrivateT = decltype(GetPrivate<T>())::type;

template<typename T>
using PublicT = decltype(GetPublic<T>())::type;

template<typename E>
concept IsID = std::is_enum_v<E>;
};

template<typename T>
auto ToPublic(T handle) {
    using Pub = Detail::PublicT<T>;
    if constexpr (Detail::IsID<T>) {
        auto ptr = static_cast<uintptr_t>(handle);
        return reinterpret_cast<Pub>(ptr);
    } else {
        return reinterpret_cast<Pub>(handle);
    }
}

template<typename T>
auto ToPrivate(T handle) {
    using Pri = Detail::PrivateT<T>;
    if constexpr (Detail::IsID<Pri>) {
        auto ptr = reinterpret_cast<uintptr_t>(handle);
        return static_cast<Pri>(ptr);
    } else {
        return reinterpret_cast<Pri>(handle);
    }
}
}
