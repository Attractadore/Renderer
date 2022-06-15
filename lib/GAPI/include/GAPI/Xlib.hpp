#pragma once
#include "GAL/GALXlib.hpp"
#include "Instance.hpp"
#include "Swapchain.hpp"

namespace R1::GAPI::Xlib {
inline Instance CreateInstance(
    ::Display* dpy,
    int screen,
    const GAL::InstanceConfig& config
) {
    HInstance instance{GAL::Xlib::CreateInstance(dpy, screen, config)};
    return Instance{std::move(instance)};
}

inline Surface CreateSurface(
    GAL::Instance instance,
    ::Display* dpy,
    ::Window window,
    GAL::SurfaceSizeCallback size_cb
) {
    HSurface surface{GAL::Xlib::CreateSurface(instance, dpy, window)};
    return Surface{std::move(surface), std::move(size_cb)};
}
}
