#pragma once
#include "Instance.hpp"
#include "Swapchain.hpp"

#include <X11/Xlib.h>

namespace R1::VK {
Instance CreateInstanceXlib(
    ::Display* dpy, int screen, const InstanceConfig& config
);

Surface CreateSurfaceXlib(
    Instance instance, ::Display* dpy, ::Window window
);
}
