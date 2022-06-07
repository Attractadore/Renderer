#pragma once
#include "GAPIRAII.hpp"

namespace R1::Rendering {
class Surface {
    HSurface            m_surface;
    SurfaceSizeCallback m_size_callback;

public:
    Surface(HSurface surface, SurfaceSizeCallback size_cb):
        m_surface{std::move(surface)},
        m_size_callback{std::move(size_cb)} {}

    GAPI::Surface get() noexcept { return m_surface.get(); }
    const SurfaceSizeCallback& GetSizeCallback() const noexcept {
        return m_size_callback;
    }
};

class Swapchain {
    HSwapchain m_swapchain;

public:
    Swapchain(
        GAPI::Context ctx,
        GAPI::Surface surface,
        SurfaceSizeCallback size_cb,
        const GAPI::SwapchainConfig& config
    ): m_swapchain{GAPI::CreateSwapchain(ctx, surface, std::move(size_cb), config)} {}

    GAPI::Swapchain get() noexcept { return m_swapchain.get(); }
};
}
