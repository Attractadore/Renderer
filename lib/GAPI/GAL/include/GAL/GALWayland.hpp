#pragma once
#include "Instance.hpp"
#include "Swapchain.hpp"

#include <wayland-client.h>

namespace R1::GAL::Wayland {
Instance CreateInstance(
    struct wl_display* display, const InstanceConfig& config
);

Surface CreateSurface(
    Instance instance, struct wl_display* display, struct wl_surface* surface
);
}
