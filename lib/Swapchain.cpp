#include "Rendering/Context.hpp"
#include "Swapchain.hpp"

namespace R1 {
namespace {
PresentMode SelectPresentMode(
    std::span<const PresentMode> present_modes
) {
    return PresentMode::FIFO;
}

Rendering::Swapchain::Config ConfigureSwapchain(
    Rendering::Context& ctx,
    Rendering::Surface& surface
) {
    auto device = ctx.GetDevice().get();
    auto surf = surface.get();

    auto desc = GAPI::GetSurfaceDescription(surf, device);
    auto present_mode = SelectPresentMode(desc.supported_present_modes);

    return {
        .present_mode = present_mode,
    };
}
}

Swapchain::Swapchain(
    Rendering::Context& ctx,
    Rendering::Surface& surface
): m_swapchain{ctx, surface, ConfigureSwapchain(ctx, surface)} {}
}
