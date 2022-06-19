#pragma once
#include "Instance.hpp"
#include "Swapchain.hpp"

#include <windows.h>

namespace R1::GAL::Win32 {
Instance CreateInstance(
    HDC hDc, const InstanceConfig& config
);

Surface CreateSurface(
    Instance instance, HWND hWnd
);
}

