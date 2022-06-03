#pragma once
#include "Instance.hpp"

#include <xcb/xcb.h>

namespace R1::VK {
Instance CreateInstanceXCB(::xcb_connection_t* c, const InstanceConfig& config, int screen = 0);
}
