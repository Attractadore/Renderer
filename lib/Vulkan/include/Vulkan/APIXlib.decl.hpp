#pragma once
#include "Types.hpp"

#include <X11/Xlib.h>

namespace R1::VK {
Instance CreateInstanceXlib(const InstanceConfig& config, ::Display* dpy, int screen = 0);
}
