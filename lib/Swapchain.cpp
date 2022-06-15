#include "GAPI/Context.hpp"
#include "Swapchain.hpp"

namespace R1 {
namespace {
GAL::PresentMode SelectPresentMode(
    std::span<const GAL::PresentMode> present_modes
) {
    return GAL::PresentMode::FIFO;
}

GAPI::Swapchain::Config ConfigureSwapchain(
    GAPI::Context& ctx,
    GAPI::Surface& surface
) {
    auto device = ctx.GetDevice().get();
    auto surf = surface.get();

    auto desc = GAL::GetSurfaceDescription(surf, device);
    auto present_mode = SelectPresentMode(desc.supported_present_modes);

    return {
        .present_mode = present_mode,
    };
}
}

Swapchain::Swapchain(
    GAPI::Context& ctx,
    GAPI::Surface& surface
): m_swapchain{ctx, surface, ConfigureSwapchain(ctx, surface)} {}
}
