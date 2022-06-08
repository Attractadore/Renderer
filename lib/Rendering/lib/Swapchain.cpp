#include "Context.hpp"
#include "Format.hpp"
#include "Swapchain.hpp"

#include <algorithm>

namespace R1::Rendering {
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
    auto key = [](CompositeAlpha composite_alpha) {
        using enum CompositeAlpha;
        switch(composite_alpha) {
            case Opaque:
                return 4;
            case Inherit:
            case PostMultiplied:
            case PreMultiplied:
                return 1;
            default:
                return 0;
        }
    };
    return std::ranges::max(composite_alphas, std::less<void>{}, key);
}


HSwapchain CreateSwapchain(
    Context& ctx, Surface& surface, const Swapchain::Config& config
) {
    auto device = ctx.GetDevice().get();
    auto surf = surface.get();

    auto desc = GAPI::GetSurfaceDescription(surf, device);
    auto surf_fmt = SelectSurfaceFormat(desc.supported_formats);
    auto image_count = std::max(desc.min_image_count, 2u);
    if (desc.max_image_count) {
        image_count = std::max(desc.max_image_count, image_count);
    }
    auto composite_alpha = SelectCompositeAlpha(desc.supported_composite_alphas);

    GAPI::SwapchainConfig cfg = {
        .image_usage = {
            .color_attachment = true,
        },
        .format = surf_fmt.format,
        .color_space = surf_fmt.color_space,
        .image_count = image_count,
        .transform = config.transform,
        .composite_alpha = composite_alpha,
        .present_mode = config.present_mode,
        .present_queue = ctx.GetGraphicsQueue(),
     };

    return HSwapchain{GAPI::CreateSwapchain(
        ctx.get(), surf, surface.GetSizeCallback(), cfg)};
}
}

Swapchain::Swapchain(
    Context& ctx,
    Surface& surface,
    const Config& config
): m_swapchain{CreateSwapchain(ctx, surface, config)} {}
}
