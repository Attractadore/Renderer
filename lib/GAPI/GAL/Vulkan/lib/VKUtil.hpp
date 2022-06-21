#pragma once
#include "VKSType.hpp"

#include <stdexcept>
#include <functional>
#include <vector>

#include <vulkan/vulkan.h>

namespace R1::GAL {
#ifndef VK_KHR_XLIB_SURFACE_EXTENSION_NAME
constexpr inline auto VK_KHR_XLIB_SURFACE_EXTENSION_NAME = "VK_KHR_xlib_surface";
#endif
#ifndef VK_KHR_XCB_SURFACE_EXTENSION_NAME
constexpr inline auto VK_KHR_XCB_SURFACE_EXTENSION_NAME = "VK_KHR_xcb_surface";
#endif
#ifndef VK_KHR_WAYLAND_SURFACE_EXTENSION_NAME
constexpr inline auto VK_KHR_WAYLAND_SURFACE_EXTENSION_NAME = "VK_KHR_wayland_surface";
#endif
#ifndef VK_KHR_WIN32_SURFACE_EXTENSION_NAME
constexpr inline auto VK_KHR_WIN32_SURFACE_EXTENSION_NAME = "VK_KHR_win32_surface";
#endif

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
}
