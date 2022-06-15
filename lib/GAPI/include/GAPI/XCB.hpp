#pragma once
#include "GAL/GALXCB.hpp"
#include "Instance.hpp"
#include "Swapchain.hpp"

namespace R1::GAPI::XCB {
inline Instance CreateInstance(
    xcb_connection_t* conn,
    int screen,
    const GAL::InstanceConfig& config
) {
    HInstance instance{GAL::XCB::CreateInstance(conn, screen, config)};
    return Instance{std::move(instance)};
}

inline Surface CreateSurface(
    GAL::Instance instance,
    xcb_connection_t* conn,
    xcb_window_t window,
    GAL::SurfaceSizeCallback size_cb
) {
    HSurface surface{GAL::XCB::CreateSurface(instance, conn, window)};
    return Surface{std::move(surface), std::move(size_cb)};
}
}
