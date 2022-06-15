#pragma once
#include "Instance.hpp"
#include "Swapchain.hpp"

#include <xcb/xcb.h>

namespace R1::GAL::XCB {
Instance CreateInstance(
    xcb_connection_t* conn, int screen, const InstanceConfig& config
);

Surface CreateSurface(
    Instance instance, xcb_connection_t* conn, xcb_window_t window
);
}
