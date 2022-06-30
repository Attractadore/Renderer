#include "R1VulkanSwapchain.hpp"
#include "GAPI/Format.hpp"

#include <boost/container/static_vector.hpp>

#include <algorithm>

namespace R1 {
namespace {
VkSurfaceFormatKHR SelectSurfaceFormat(std::span<const VkSurfaceFormatKHR> surf_fmts) {
    auto it = std::ranges::find_if(surf_fmts,
        [](const VkSurfaceFormatKHR& sf) {
            return GAPI::IsSRGBFormat(static_cast<GAL::Format>(sf.format));
        });
    return it != surf_fmts.end() ? *it: surf_fmts.front();
};

VkCompositeAlphaFlagBitsKHR SelectCompositeAlpha(
    VkCompositeAlphaFlagsKHR composite_alphas
) {
    if (composite_alphas & VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR) {
        return VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    } else {
        return static_cast<VkCompositeAlphaFlagBitsKHR>(
            std::bit_floor(composite_alphas));
    }
}

GAL::Vulkan::SwapchainConfig ConfigureSwapchain(
    GAPI::Context& ctx, VkSurfaceKHR surface, const VulkanSwapchainConfig& config
) {
    auto device = ctx.GetDevice().get();

    auto desc = GAL::Vulkan::GetSurfaceDescription(surface, device);
    auto surf_fmt = SelectSurfaceFormat(desc.supported_formats);
    auto image_count = std::max(desc.min_image_count, 2u);
    if (desc.max_image_count) {
        image_count = std::max(desc.max_image_count, image_count);
    }
    auto composite_alpha = SelectCompositeAlpha(desc.supported_composite_alpha);

    return {
        .format = surf_fmt.format,
        .color_space = surf_fmt.colorSpace,
        .image_usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT,
        .image_count = image_count,
        .composite_alpha = composite_alpha,
        .present_mode = config.present_mode,
        .clipped = true,
    };
}

struct RecordBlitConfig {
    GAL::Image image;
    GAL::Image swc_image;
    GAL::ImageLayout begin_layout;
    GAL::ImageLayout end_layout;
    unsigned image_width;
    unsigned image_height;
    unsigned swc_width;
    unsigned swc_height;
    GAL::Filter blit_filter;
};

void RecordCommandsBlit(
    GAL::Context ctx,
    GAL::CommandBuffer cmd_buffer,
    const RecordBlitConfig& config
) {
    GAL::BeginCommandBuffer(ctx, cmd_buffer, {});

    {
        GAL::ImageSubresourceRange srng = {
            .aspects = GAL::ImageAspect::Color,
            .mip_level_count = 1,
            .array_layer_count = 1,
        };
        std::array<GAL::ImageBarrier, 2> barriers;
        auto& to_transfer = barriers[0] = {
            .memory_barrier = {
                .src_stages = GAL::PipelineStage::Blit,
                .dst_stages = GAL::PipelineStage::Blit,
                .dst_accesses = GAL::MemoryAccess::TransferRead,
            },
            .old_layout = config.begin_layout,
            .new_layout = GAL::ImageLayout::TransferSRC,
            .image = config.image,
            .subresource_range = srng,
        };
        auto& swc_to_transfer = barriers[1] = {
            .memory_barrier = {
                .src_stages = GAL::PipelineStage::Blit,
                .dst_stages = GAL::PipelineStage::Blit,
                .dst_accesses = GAL::MemoryAccess::TransferWrite,
            },
            .new_layout = GAL::ImageLayout::TransferDST,
            .image = config.swc_image,
            .subresource_range = srng,
        };
        GAL::DependencyConfig dep_cfg = {
            .image_barriers = barriers,
        };
        GAL::CmdPipelineBarrier(ctx, cmd_buffer, dep_cfg);
    }

    {
        GAL::ImageSubresourceLayers subresource = {
            .aspects = GAL::ImageAspect::Color,
            .array_layer_count = 1,
        };
        GAL::BlitRegion region = {
            .src_subresource = subresource,
            .src_offsets = {{{}, {config.image_width, config.image_height, 1}}},
            .dst_subresource = subresource,
            .dst_offsets = {{{}, {config.swc_width, config.swc_height, 1}}},
        };
        GAL::CmdBlitImage(ctx, cmd_buffer, {
            .src_image = config.image,
            .src_layout = GAL::ImageLayout::TransferSRC,
            .dst_image = config.swc_image,
            .dst_layout = GAL::ImageLayout::TransferDST,
            .regions = {&region, 1},
            .filter = config.blit_filter,
        });
    }

    {
        GAL::ImageSubresourceRange srng = {
            .aspects = GAL::ImageAspect::Color,
            .mip_level_count = 1,
            .array_layer_count = 1,
        };
        boost::container::static_vector<GAL::ImageBarrier, 2> barriers;
        if (config.end_layout != GAL::ImageLayout::Undefined) {
            auto& from_transfer = barriers.emplace_back() = {
                .memory_barrier = {
                    .src_stages = GAL::PipelineStage::Blit,
                },
                .old_layout = GAL::ImageLayout::TransferSRC,
                .new_layout = config.end_layout,
                .image = config.image,
                .subresource_range = srng,
            };
        }
        auto& swc_from_transfer = barriers.emplace_back() = {
            .memory_barrier = {
                .src_stages = GAL::PipelineStage::Blit,
                .src_accesses = GAL::MemoryAccess::TransferWrite,
            },
            .old_layout = GAL::ImageLayout::TransferDST,
            .new_layout = GAL::ImageLayout::Present,
            .image = config.swc_image,
            .subresource_range = srng,
        };
        GAL::DependencyConfig dep_cfg = {
            .image_barriers = barriers,
        };
        GAL::CmdPipelineBarrier(ctx, cmd_buffer, dep_cfg);
    }

    GAL::EndCommandBuffer(ctx, cmd_buffer);
}
}

VulkanSwapchain::VulkanSwapchain(
    R1::VulkanContext& ctx,
    VkSurfaceKHR surface,
    GAL::Vulkan::SurfaceSizeCallback size_cb,
    const VulkanSwapchainConfig& config
):  m_ctx{ctx.get()},
    m_swapchain{GAL::Vulkan::CreateSwapchain(
        m_ctx.get(),
        surface,
        std::move(size_cb),
        ConfigureSwapchain(m_ctx, surface, config)
    )},
    m_cmd_pool{m_ctx.get(), GAL::CreateCommandPool(m_ctx.get(), {
        .queue_family = m_ctx.GetGraphicsQueueFamily(),
    })}
{
    CreateSyncs();   
}

void VulkanSwapchain::AcquireImage() {
    using enum GAL::Vulkan::SwapchainStatus;
    auto ctx = m_ctx.get();
    auto swc = m_swapchain.get();
    m_acquire_idx = (m_acquire_idx + 1) % GetImageCount();
    auto fence = GetCurrentAcquireFence();

    // Wait for acquire semaphore to be available/unsignaled
    GAL::Vulkan::WaitForFences(
        ctx, {&fence, 1}, true, std::chrono::nanoseconds{UINT64_MAX});
    GAL::Vulkan::ResetFences(ctx, {&fence, 1});

    auto [idx, status] = GAL::Vulkan::AcquireImage(
        swc, GetCurrentAcquireSemaphore());
    if (status == Optimal) {
        m_current_image_idx = idx;
        return;
    }

    // Wait for all draws and presentation operations to finish so a 
    // resize can be performed.
    GAL::QueueWaitIdle(ctx, GetPresentQueue());
    do {
        Resize();
        std::tie(m_current_image_idx, status) =
            GAL::Vulkan::AcquireImage(swc, GetCurrentAcquireSemaphore());
    } while (status != Optimal);
}

void VulkanSwapchain::ConfigPresentForImages(
    std::span<const GAL::Image> images,
    const VulkanSwapchainPresentImageConfig& config
) {
    auto ctx = m_ctx.get();
    auto swc_cnt = GetImageCount();
    auto [swc_w, swc_h] = Size();

    m_cmd_buffers = Detail::CommandBufferSet{
        ctx, m_cmd_pool.get(), images.size() * swc_cnt};
    m_cmd_buffer_map.clear();

    unsigned num_recorded = 0;
    for (auto image: images) {
        for (unsigned i = 0; i < swc_cnt; i++, num_recorded++) {
            auto swc_image = GetImage(i);
            auto cmd_buffer = m_cmd_buffers[num_recorded];
            RecordCommandsBlit(ctx, cmd_buffer, {
                .image = image,
                .swc_image = swc_image,
                .begin_layout = config.end_layout,
                .end_layout = config.start_layout,
                .image_width = config.width,
                .image_height = config.height,
                .swc_width = swc_w,
                .swc_height = swc_h,
                .blit_filter = GAL::Filter::Nearest,
            });
            m_cmd_buffer_map.emplace(
                std::piecewise_construct,
                std::forward_as_tuple(image, swc_image),
                std::forward_as_tuple(cmd_buffer));
        }
    }
}

void VulkanSwapchain::PresentImage(
    GAL::Image image,
    GAL::Semaphore semaphore,
    GAL::SemaphorePayload wait_value,
    GAL::SemaphorePayload signal_value
) {
    auto ctx = m_ctx.get();
    auto q = GetPresentQueue();
    auto fence = GetCurrentPresentFence();

    // Wait for present semaphore to be unsignaled
    GAL::Vulkan::WaitForFences(
        ctx, {&fence, 1}, true, std::chrono::nanoseconds{UINT64_MAX});
    GAL::Vulkan::ResetFences(ctx, {&fence, 1});

    SubmitTransferCommands(image, semaphore, wait_value, signal_value);

    using enum GAL::Vulkan::SwapchainStatus;
    auto status = GAL::Vulkan::PresentImage(
        q, m_swapchain.get(),
        m_current_image_idx, GetCurrentPresentSemaphore());
    if (status == Optimal) {
        GAL::Vulkan::QueueSubmit(ctx, q, {}, fence);
    } else {
        GAL::QueueWaitIdle(ctx, q);
        Resize();
    }
}

void VulkanSwapchain::CreateSyncs() {
    auto ctx = m_ctx.get();
    auto cnt = GAL::Vulkan::GetSwapchainImageCount(m_swapchain.get());
    m_syncs.resize(cnt);
    std::ranges::generate(m_syncs, [&] {
        return Syncs {
            .acquire_semaphore{ctx,
                GAL::Vulkan::CreateBinarySemaphore(ctx)},
            .acquire_fence{ctx,
                GAL::Vulkan::CreateFence(ctx, true)},
            .present_semaphore{ctx,
                GAL::Vulkan::CreateBinarySemaphore(ctx)},
            .present_fence{ctx,
                GAL::Vulkan::CreateFence(ctx, true)},
        };
    });
}

void VulkanSwapchain::Resize() {
    GAL::Vulkan::ResizeSwapchain(m_swapchain.get());
    CreateSyncs();
    m_acquire_idx = 0;
}

void VulkanSwapchain::SubmitTransferCommands(
    GAL::Image image,
    GAL::Semaphore image_semaphore,
    GAL::SemaphorePayload wait_value,
    GAL::SemaphorePayload signal_value
) {
    std::array<GAL::SemaphoreSubmitConfig, 2> wait_states;
    std::array<GAL::SemaphoreSubmitConfig, 2> signal_states;
    auto& acquire_wait_state = wait_states[0] = {
        .state = {
            .semaphore = GetCurrentAcquireSemaphore(),
        },
        .stages = GAL::PipelineStage::Blit,
    };
    auto& image_wait_state = wait_states[1] = {
        .state = {
            .semaphore = image_semaphore,
            .value = wait_value,
        },
        .stages = GAL::PipelineStage::Blit,
    };
    auto& present_signal_state = signal_states[0] = {
        .state = {
            .semaphore = GetCurrentPresentSemaphore(),
        },
    };
    auto& image_signal_state = signal_states[1] = {
        .state = {
            .semaphore = image_semaphore,
            .value = signal_value,
        },
    };
    auto cmd_buffer = GetCommandBuffer(image, GetCurrentImage());
    GAL::QueueSubmitConfig submit = {
        .wait_semaphores = wait_states,
        .signal_semaphores = signal_states,
        .command_buffers = {&cmd_buffer, 1},
    };
    GAL::Vulkan::QueueSubmit(
        m_ctx.get(), GetPresentQueue(),
        {&submit, 1}, GetCurrentAcquireFence());
}

GAL::CommandBuffer VulkanSwapchain::GetCommandBuffer(GAL::Image image, GAL::Image swc_image) const noexcept {
    return m_cmd_buffer_map.find({image, swc_image})->second;
}
}
