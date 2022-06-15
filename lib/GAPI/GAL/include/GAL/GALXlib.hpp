#pragma once
#include "Instance.hpp"
#include "Swapchain.hpp"

#include <X11/Xlib.h>

namespace R1::GAL::Xlib {
Instance CreateInstance(
    ::Display* dpy, int screen, const InstanceConfig& config
);

Surface CreateSurface(
    Instance instance, ::Display* dpy, ::Window window
);
}
