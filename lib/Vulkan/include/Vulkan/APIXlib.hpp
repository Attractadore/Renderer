#pragma once
#include "Instance.hpp"

#include <X11/Xlib.h>

namespace R1::VK {
Instance CreateInstanceXlib(::Display* dpy, const InstanceConfig& config, int screen = 0);
}
