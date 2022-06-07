#pragma once
#include "GAPIXlib.hpp"
#include "Instance.hpp"
#include "Swapchain.hpp"

namespace R1::Rendering::Xlib {
inline Instance CreateInstance(
    ::Display* dpy,
    int screen,
    const InstanceConfig& config
) {
    HInstance instance{GAPI::CreateInstanceXlib(dpy, screen, config)};
    return Instance{std::move(instance)};
}

inline Surface CreateSurface(
    GAPI::Instance instance,
    ::Display* dpy,
    ::Window window,
    SurfaceSizeCallback size_cb
) {
    HSurface surface{
        instance, GAPI::CreateSurfaceXlib(instance, dpy, window)};
    return Surface{std::move(surface), std::move(size_cb)};
}
}
