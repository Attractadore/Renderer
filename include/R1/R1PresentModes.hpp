#pragma once
#include "R1PresentModes.h"

namespace R1 {
enum class PresentMode {
    NoVSync             = R1_PRESENT_MODE_NO_VSYNC,
    VSync               = R1_PRESENT_MODE_VSYNC,
    AdaptiveVSync       = R1_PRESENT_MODE_ADAPTIVE_VSYNC,
    MultibufferedVSync  = R1_PRESENT_MODE_MULTIBUFFERED_VSYNC,
};
};
