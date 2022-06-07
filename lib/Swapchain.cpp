#include "Rendering/Context.hpp"
#include "Swapchain.hpp"

namespace R1 {
namespace {
Rendering::Swapchain CreateSwapchain(
    Rendering::Context& ctx,
    Rendering::Surface& surface
) {
    GAPI::SwapchainConfig config = {
    };
    return Rendering::Swapchain{
        ctx.get(), surface.get(), surface.GetSizeCallback(), config};
}
}

Swapchain::Swapchain(
    Rendering::Context& ctx,
    Rendering::Surface& surface
): m_swapchain{CreateSwapchain(ctx, surface)} {}
}
