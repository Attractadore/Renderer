#pragma once
#include "GALRAII.hpp"

namespace R1::GAPI {
class Context;

class Surface {
    HSurface                    m_surface;
    GAL::SurfaceSizeCallback    m_size_callback;

public:
    Surface(HSurface surface, GAL::SurfaceSizeCallback size_cb):
        m_surface{std::move(surface)},
        m_size_callback{std::move(size_cb)} {}

    GAL::Surface get() noexcept { return m_surface.get(); }
    const GAL::SurfaceSizeCallback& GetSizeCallback() const noexcept {
        return m_size_callback;
    }
};

class Swapchain {
    GAL::SwapchainConfig    m_config;
    HSwapchain              m_swapchain;

public:
    struct Config {
        GAL::CompositeAlpha  composite_alpha;
        GAL::PresentMode     present_mode;
    };

    Swapchain(
        Context& ctx,
        Surface& surface,
        const Config& config
    );

    GAL::Swapchain get() noexcept { return m_swapchain.get(); }
    GAL::Format GetFormat() const noexcept { return m_config.format; }
};
}
