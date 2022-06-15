#pragma once
#include "GAPIXCB.hpp"
#include "Instance.hpp"
#include "Swapchain.hpp"

namespace R1::Rendering::XCB {
inline Instance CreateInstance(
    xcb_connection_t* conn,
    int screen,
    const InstanceConfig& config
) {
    HInstance instance{GAPI::CreateInstanceXCB(conn, screen, config)};
    return Instance{std::move(instance)};
}

inline Surface CreateSurface(
    GAPI::Instance instance,
    xcb_connection_t* conn,
    xcb_window_t window,
    SurfaceSizeCallback size_cb
) {
    HSurface surface{GAPI::CreateSurfaceXCB(instance, conn, window)};
    return Surface{std::move(surface), std::move(size_cb)};
}
}
