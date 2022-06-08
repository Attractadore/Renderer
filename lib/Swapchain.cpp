#include "Rendering/Context.hpp"
#include "Rendering/Format.hpp"
#include "Swapchain.hpp"

#include <algorithm>

namespace R1 {
namespace {
SurfaceFormat SelectSurfaceFormat(std::span<const SurfaceFormat> surf_fmts) {
    auto it = std::ranges::find_if(surf_fmts,
        [](const SurfaceFormat& sf) {
            return Rendering::IsSRGBFormat(sf.format);
        });
    return it != surf_fmts.end() ? *it: surf_fmts.front();
};

CompositeAlpha SelectCompositeAlpha(
    std::span<const CompositeAlpha> composite_alphas
) {
    return composite_alphas.front();
}

PresentMode SelectPresentMode(
    std::span<const PresentMode> present_modes
) {
    return PresentMode::FIFO;
}

Rendering::Swapchain CreateSwapchain(
    Rendering::Context& ctx,
    Rendering::Surface& surface
) {
    auto device = ctx.GetDevice().get();
    auto surf = surface.get();

    auto desc = GAPI::GetSurfaceDescription(surf, device);
    auto surf_fmt = SelectSurfaceFormat(desc.supported_formats);
    auto composite_alpha = SelectCompositeAlpha(desc.supported_composite_alphas);
    auto present_mode = SelectPresentMode(desc.supported_present_modes);
    auto transform = GAPI::GetSurfaceCurrentTransform(surf, device);

    GAPI::SwapchainConfig config = {
        .image_usage = {
            .color_attachment = true,
        },
        .format = surf_fmt.format,
        .color_space = surf_fmt.color_space,
        .image_count = std::max(desc.min_image_count, 2u),
        .transform = transform,
        .composite_alpha = composite_alpha,
        .present_mode = present_mode,
        .present_queue = ctx.GetGraphicsQueue(),
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
