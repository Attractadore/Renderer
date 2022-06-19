#pragma once
#include "GAL/GALWayland.hpp"
#include "Instance.hpp"
#include "Swapchain.hpp"

namespace R1::GAPI::Wayland {
inline Instance CreateInstance(
    struct wl_display* display,
    const GAL::InstanceConfig& config
) {
    HInstance instance{GAL::Wayland::CreateInstance(display, config)};
    return Instance{std::move(instance)};
}

inline Surface CreateSurface(
    GAL::Instance instance,
    struct wl_display* display,
    struct wl_surface* surface,
    GAL::SurfaceSizeCallback size_cb
) {
    HSurface surf{GAL::Wayland::CreateSurface(instance, display, surface)};
    return Surface{std::move(surf), std::move(size_cb)};
}
}
