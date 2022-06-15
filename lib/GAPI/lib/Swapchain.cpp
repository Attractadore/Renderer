#include "Context.hpp"
#include "Format.hpp"
#include "Swapchain.hpp"

#include <algorithm>

namespace R1::GAPI {
namespace {
GAL::SurfaceFormat SelectSurfaceFormat(std::span<const GAL::SurfaceFormat> surf_fmts) {
    auto it = std::ranges::find_if(surf_fmts,
        [](const GAL::SurfaceFormat& sf) {
            return IsSRGBFormat(sf.format);
        });
    return it != surf_fmts.end() ? *it: surf_fmts.front();
};

GAL::CompositeAlpha SelectCompositeAlpha(
    std::span<const GAL::CompositeAlpha> composite_alphas
) {
    auto key = [](GAL::CompositeAlpha composite_alpha) {
        using enum GAL::CompositeAlpha;
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


GAL::SwapchainConfig ConfigureSwapchain(
    Context& ctx, Surface& surface, const Swapchain::Config& config
) {
    auto device = ctx.GetDevice().get();
    auto surf = surface.get();

    auto desc = GAL::GetSurfaceDescription(surf, device);
    auto surf_fmt = SelectSurfaceFormat(desc.supported_formats);
    auto image_count = std::max(desc.min_image_count, 2u);
    if (desc.max_image_count) {
        image_count = std::max(desc.max_image_count, image_count);
    }
    auto composite_alpha = SelectCompositeAlpha(desc.supported_composite_alphas);

    return {
        .format = surf_fmt.format,
        .color_space = surf_fmt.color_space,
        .image_usage = GAL::ImageUsage::ColorAttachment,
        .image_count = image_count,
        .composite_alpha = composite_alpha,
        .present_mode = config.present_mode,
        .present_queue = ctx.GetGraphicsQueue(),
        .clipped = true,
    };
}
}

Swapchain::Swapchain(
    Context& ctx,
    Surface& surface,
    const Config& config
):  m_config{ConfigureSwapchain(ctx, surface, config)},
    m_swapchain{GAL::CreateSwapchain(
        ctx.get(), surface.get(), surface.GetSizeCallback(), m_config
    )} {} 
}
