#pragma once
#include "GAL/GALWin32.hpp"
#include "Instance.hpp"
#include "Swapchain.hpp"

namespace R1::GAPI::Win32 {
inline Instance CreateInstance(
    HDC hDc, const GAL::InstanceConfig& config
) {
    HInstance instance{GAL::Win32::CreateInstance(hDc, config)};
    return Instance{std::move(instance)};
}

inline Surface CreateSurface(
    GAL::Instance instance,
    HWND hWnd,
    GAL::SurfaceSizeCallback size_cb
) {
    HSurface surf{GAL::Win32::CreateSurface(instance, hWnd)};
    return Surface{std::move(surf), std::move(size_cb)};
}
}
