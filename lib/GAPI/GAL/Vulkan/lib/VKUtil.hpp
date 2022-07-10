#pragma once
#include "VKSType.hpp"

#include <boost/container/small_vector.hpp>
#include <vulkan/vulkan.h>

#include <functional>
#include <stdexcept>
#include <vector>

namespace R1::GAL {
namespace Detail {
template<typename F, typename... Args>
    requires std::invocable<F, Args&&...>
VkResult invoke(F&& f, Args&&... args) {
    using R = std::invoke_result_t<F, Args&&...>;
    if constexpr (std::same_as<R, void>) {
        std::invoke(std::forward<F>(f), std::forward<Args>(args)...);
        return VK_SUCCESS;
    } else {
        return std::invoke(std::forward<F>(f), std::forward<Args>(args)...);
    }
}
}

template<typename T, typename P, typename H, std::invocable<P, H, uint32_t*, T*> F>
std::vector<T> Enumerate(P parent, H handle, F f) {
    uint32_t cnt = 0;
    if (!Detail::invoke(f, parent, handle, &cnt, static_cast<T*>(nullptr))) {
        std::vector<T> res(cnt);
        if (!Detail::invoke(f, parent, handle, &cnt, res.data())) {
            res.resize(cnt);
            return res;
        }
    }
    throw std::runtime_error{"Vulkan: failed to enumerate"};
}

template<typename T, typename H, std::invocable<H, uint32_t*, T*> F>
std::vector<T> Enumerate(H handle, F f) {
    uint32_t cnt = 0;
    if (!Detail::invoke(f, handle, &cnt, static_cast<T*>(nullptr))) {
        std::vector<T> res(cnt);
        if (!Detail::invoke(f, handle, &cnt, res.data())) {
            res.resize(cnt);
            return res;
        }
    }
    throw std::runtime_error{"Vulkan: failed to enumerate"};
}

template<typename T, std::invocable<uint32_t*, T*> F>
std::vector<T> Enumerate(F f) {
    uint32_t cnt = 0;
    if (!Detail::invoke(f, &cnt, static_cast<T*>(nullptr))) {
        std::vector<T> res(cnt);
        if (!Detail::invoke(f, &cnt, res.data())) {
            res.resize(cnt);
            return res;
        }
    }
    throw std::runtime_error{"Vulkan: failed to enumerate"};
}

template<typename E, typename F>
std::vector<E> ExtractEnum(
    F flags
) {
    std::vector<E> enums;
    for (VkSurfaceTransformFlagsKHR i = 1; i; i <<= 1) {
        if (flags & i) {
            enums.push_back(static_cast<E>(i));
        }
    };
    return enums;
}

inline void ThrowIfFailed(VkResult r, const char* msg) {
    if (r) {
        throw std::runtime_error{msg};
    }
}

// Preallocate 4k for translating data structures to Vulkan
constexpr size_t DefaultSmallBufferSize = 1 << 12; 

template<typename T, size_t Size = DefaultSmallBufferSize>
constexpr size_t SmallBufferCount = Size / sizeof(T); 

template<typename T>
using DefaultSmallVector = boost::container::small_vector<T, SmallBufferCount<T>>;
}
