#pragma once
#include "GAPIRAII.hpp"

namespace R1::Rendering {
class Context;

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
    GAPI::SwapchainConfig   m_config;
    HSwapchain              m_swapchain;

public:
    struct Config {
        CompositeAlpha  composite_alpha;
        PresentMode     present_mode;
    };

    Swapchain(
        Rendering::Context& ctx,
        Rendering::Surface& surface,
        const Config& config
    );

    GAPI::Swapchain get() noexcept { return m_swapchain.get(); }
    Format GetFormat() const noexcept { return m_config.format; }
};
}
