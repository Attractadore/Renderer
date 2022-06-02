#pragma once
#include "Types.hpp"

#include <xcb/xcb.h>

namespace R1::VK {
Instance CreateInstanceXCB(const InstanceConfig& config, ::xcb_connection_t* c, int screen = 0);
}
