#include "Context.hpp"
#include "Scene.hpp"
#include "Swapchain.hpp"

#include <algorithm>
#include <filesystem>
#include <fstream>

namespace R1 {
namespace {
std::vector<std::byte> loadShader(const std::string& path) {
    std::ifstream f{path, std::ios_base::binary};
    auto sz = std::filesystem::file_size(path);
    std::vector<std::byte> data(sz);
    f.read(reinterpret_cast<char*>(data.data()), sz);
    return data;
}

GAL::PipelineLayout createPipelineLayout(GAL::Context ctx) {
    return GAL::CreatePipelineLayout(ctx, {});
}

GAL::Pipeline createPipeline(
    GAL::Context ctx,
    GAL::PipelineLayout layout,
    GAL::ShaderModule vert_module,
    GAL::ShaderModule frag_module,
    GAL::Format color_fmt
) {
    GAL::GraphicsPipelineConfigurator gpc;

    gpc.SetLayout(layout);

    GAL::ShaderStageConfig vert_stage = {
        .module = vert_module,
        .entry_point = "main",
    };
    GAL::VertexInputConfig vert_input = {};
    GAL::InputAssemblyConfig input_assembly = {
        .primitive_topology = GAL::PrimitiveTopology::TriangleList,
    };
    gpc.SetVertexShaderState(vert_stage, vert_input, {}, {}, input_assembly);

    GAL::RasterizationConfig rast = {
        .polygon_mode = GAL::PolygonMode::Fill,
        .cull_mode = GAL::CullMode::None,
        .line_width = 1.0f,
    };
    GAL::MultisampleConfig ms = {
        .sample_count = 1,
    };
    gpc.SetRasterizationState(rast, ms);

    GAL::ShaderStageConfig frag_stage = {
        .module = frag_module,
        .entry_point = "main",
    };
    GAL::ColorBlendConfig blend = {};
    GAL::ColorAttachmentConfig att = {
        .format = color_fmt,
        .color_mask =
            GAL::ColorComponent::R |
            GAL::ColorComponent::G |
            GAL::ColorComponent::B |
            GAL::ColorComponent::A,
    };
    gpc.SetFragmentShaderState(frag_stage, blend, {&att, 1});
    gpc.FinishCurrent();

    GAL::Pipeline pipeline = nullptr;
    GAL::CreateGraphicsPipelines(ctx, nullptr, gpc.FinishAll(), &pipeline);

    return pipeline;
}

GAL::ImageView createSwapchainImageView(
    GAL::Context ctx, GAL::Format fmt, GAL::Image img 
) {
    GAL::ImageViewConfig config = {
        .type = GAL::ImageViewType::D2,
        .format = fmt,
        .subresource_range = {
            .aspects = GAL::ImageAspect::Color,
            .first_mip_level = 0,
            .mip_level_count = 1,
            .first_array_layer = 0,
            .array_layer_count = 1,
        },
    };
    return GAL::CreateImageView(ctx, img, config);
}

std::vector<GAL::ImageView> createSwapchainImageViews(
    GAL::Context ctx, GAL::Format fmt, GAL::Swapchain swc
) {
    std::vector<GAL::ImageView> views(GAL::GetSwapchainImageCount(swc));
    for (size_t i = 0; i < views.size(); i++) {
        views[i] = createSwapchainImageView(ctx, fmt, GAL::GetSwapchainImage(swc, i));
    }
    return views;
}

GAL::CommandPool createCommandPool(GAL::Context ctx, GAL::QueueFamily::ID qf) {
    GAL::CommandPoolConfig config = {
        .flags =
            GAL::CommandPoolConfigOption::Transient |
            GAL::CommandPoolConfigOption::AllowCommandBufferReset,
        .queue_family = qf,
    };
    return GAL::CreateCommandPool(ctx, config);
}

std::vector<GAL::CommandBuffer> createCommandBuffers(
    GAL::Context ctx, GAL::CommandPool pool, unsigned n
) {
    std::vector<GAL::CommandBuffer> cmd_buffers(n);
    GAL::AllocateCommandBuffers(ctx, pool, cmd_buffers);
    return cmd_buffers;
}

}

struct Scene::Impl {
    GAL::Context                    ctx;
    GAL::Swapchain                  swapchain;
    GAL::QueueFamily::ID            queue_family;
    GAL::Queue                      queue;
    GAL::Format                     color_fmt;
    GAL::Pipeline                   pipeline;
    std::vector<GAL::ImageView>     image_views;
    unsigned                        frame_index = 0;
    static constexpr auto           frame_count = 2;
    GAL::CommandPool                command_pool;
    std::vector<GAL::CommandBuffer> command_buffers;

    static constexpr auto           InfiniteTimeout = std::chrono::nanoseconds{UINT64_MAX};
    static constexpr auto           signal_cnt = 2;

    GAL::Semaphore                  semaphore;

    GAL::SemaphorePayload           old_acquire_semaphore_value = 0;
    GAL::SemaphorePayload           old_draw_semaphore_value = old_acquire_semaphore_value + 1;

    GAL::SemaphorePayload           last_semaphore_value = frame_count * signal_cnt - 1;

    GAL::SemaphorePayload           last_draw_semaphore_value = last_semaphore_value - 1;
    GAL::SemaphorePayload           last_acquire_semaphore_value = last_draw_semaphore_value - 1;

    ~Impl() {
        GAL::ContextWaitIdle(ctx);
        GAL::DestroySemaphore(ctx, semaphore);
        GAL::FreeCommandBuffers(ctx, command_pool, command_buffers);
        GAL::DestroyCommandPool(ctx, command_pool);
        std::ranges::for_each(image_views, [&] (auto v) { GAL::DestroyImageView(ctx, v); });
        GAL::DestroyPipeline(ctx, pipeline);
    }
};

Scene::Scene(Context& ctx, Swapchain& swapchain):
    m_swapchain{swapchain},
    pimpl{std::make_unique<Impl>()}
{
    auto& swc = m_swapchain.get().get();
    pimpl->ctx = ctx.get().get();
    pimpl->swapchain = swc.get();
    pimpl->queue_family = ctx.get().GetGraphicsQueueFamily();
    pimpl->queue = ctx.get().GetGraphicsQueue();
    pimpl->color_fmt = swc.GetFormat();
    {
        auto vert_code = loadShader("vert.spv");
        auto frag_code = loadShader("frag.spv");
        auto vert_module = GAL::CreateShaderModule(pimpl->ctx, { .code = vert_code } );
        auto frag_module = GAL::CreateShaderModule(pimpl->ctx, { .code = frag_code } );
        auto layout = createPipelineLayout(pimpl->ctx);
        pimpl->pipeline = createPipeline(pimpl->ctx, layout, vert_module, frag_module, pimpl->color_fmt);
        GAL::DestroyPipelineLayout(pimpl->ctx, layout);
        GAL::DestroyShaderModule(pimpl->ctx, vert_module);
        GAL::DestroyShaderModule(pimpl->ctx, frag_module);
    }
    {
        pimpl->image_views = createSwapchainImageViews(pimpl->ctx, pimpl->color_fmt, pimpl->swapchain);
    }
    pimpl->command_pool = createCommandPool(
        pimpl->ctx, pimpl->queue_family
    );
    pimpl->command_buffers = createCommandBuffers(
        pimpl->ctx, pimpl->command_pool, pimpl->frame_count
    );
    pimpl->semaphore = GAL::CreateSemaphore(
        pimpl->ctx, { .initial_value = pimpl->last_semaphore_value, });
}

void Scene::Draw() {
    auto ctx = pimpl->ctx;
    auto swc = pimpl->swapchain;
    auto idx = pimpl->frame_index;
    auto sem = pimpl->semaphore;

    {
        GAL::SemaphoreState wait_state = {
            .semaphore = sem,
            .value = pimpl->old_draw_semaphore_value,
        };
        GAL::WaitForSemaphores(ctx, {&wait_state, 1}, true, pimpl->InfiniteTimeout);
    }

    auto resize_swapchain = [&] (GAL::SwapchainStatus status) {
        GAL::ContextWaitIdle(ctx);
        std::ranges::for_each(pimpl->image_views, [&] (auto v) { GAL::DestroyImageView(ctx, v); });
        GAL::ResizeSwapchain(swc);
        pimpl->image_views = createSwapchainImageViews(ctx, pimpl->color_fmt, swc);
    };

    pimpl->last_acquire_semaphore_value = ++pimpl->last_semaphore_value;
    auto img_idx = [&] {
    while(true) {
        GAL::SemaphoreState signal_state = {
            .semaphore = sem,
            .value = pimpl->last_acquire_semaphore_value,
        };
        auto [img_idx, status] = GAL::AcquireImage(swc, &signal_state);
        if (status != GAL::SwapchainStatus::Optimal) {
            resize_swapchain(status);
        } else {
            return img_idx;
        };
    }} ();

    auto [img_w, img_h] = GAL::GetSwapchainSize(swc);
    auto img = GAL::GetSwapchainImage(swc, img_idx);
    auto view = pimpl->image_views[img_idx];

    auto cmd_buffer = pimpl->command_buffers[idx];

    GAL::CommandBufferBeginConfig begin_config = {
        .usage = GAL::CommandBufferUsage::OneTimeSubmit,
    };
    GAL::BeginCommandBuffer(cmd_buffer, begin_config);

    {
        GAL::ImageBarrier from_present = {
            .memory_barrier = {
                .src_stages = GAL::PipelineStage::ColorAttachmentOutput,
                .dst_stages = GAL::PipelineStage::ColorAttachmentOutput,
                .dst_accesses = GAL::MemoryAccess::ColorAttachmentWrite,
            },
            .new_layout = GAL::ImageLayout::Attachment,
            .image = img,
            .subresource_range = {
                .aspects = GAL::ImageAspect::Color,
                .first_mip_level = 0,
                .mip_level_count = 1,
                .first_array_layer = 0,
                .array_layer_count = 1,
            },
        };
        GAL::DependencyConfig config = {
            .image_barriers{&from_present, 1},
        };
        GAL::CmdPipelineBarrier(cmd_buffer, config);
    }

    GAL::ClearValue clear_color = {0.2f, 0.2f, 0.8f, 1.0f};
    GAL::RenderingAttachment color_attachment = {
        .view = view,
        .layout = GAL::ImageLayout::Attachment,
        .load_op = GAL::AttachmentLoadOp::Clear,
        .store_op = GAL::AttachmentStoreOp::Store,
        .clear_value = clear_color,
    };

    GAL::RenderingConfig rendering_config = {
        .render_area = { .width = img_w, .height = img_h },
        .color_attachments{&color_attachment, 1},
    };

    GAL::CmdBeginRendering(cmd_buffer, rendering_config);

    {
        GAL::Viewport viewport = {
            .width = static_cast<float>(img_w),
            .height = static_cast<float>(img_h),
            .min_depth = 0.0f,
            .max_depth = 1.0f,
        };
        GAL::Rect2D scissor = {
            .width = img_w,
            .height = img_h,
        };
        GAL::CmdSetViewports(cmd_buffer, {&viewport, 1});
        GAL::CmdSetScissors(cmd_buffer, {&scissor, 1});
    }

    GAL::CmdBindGraphicsPipeline(cmd_buffer, pimpl->pipeline);
    GAL::DrawConfig draw_config = {
        .vertex_count = 3,
        .instance_count = 1,
    };
    GAL::CmdDraw(cmd_buffer, draw_config);

    GAL::CmdEndRendering(cmd_buffer);

    {
        GAL::ImageBarrier to_present = {
            .memory_barrier = {
                .src_stages = GAL::PipelineStage::ColorAttachmentOutput,
                .src_accesses = GAL::MemoryAccess::ColorAttachmentWrite,
            },
            .old_layout = GAL::ImageLayout::Attachment,
            .new_layout = GAL::ImageLayout::Present,
            .image = img,
            .subresource_range = {
                .aspects = GAL::ImageAspect::Color,
                .first_mip_level = 0,
                .mip_level_count = 1,
                .first_array_layer = 0,
                .array_layer_count = 1,
            },
        };
        GAL::DependencyConfig config = {
            .image_barriers{&to_present, 1},
        };
        GAL::CmdPipelineBarrier(cmd_buffer, config);
    }

    GAL::EndCommandBuffer(cmd_buffer);

    {
        GAL::SemaphoreSubmitConfig wait_config = {
            .state = {
                .semaphore = sem,
                .value = pimpl->last_acquire_semaphore_value,
            },
            .stages = GAL::PipelineStage::ColorAttachmentOutput,
        };
        pimpl->last_draw_semaphore_value = ++pimpl->last_semaphore_value;
        GAL::SemaphoreSubmitConfig signal_config = {
            .state = {
                .semaphore = sem,
                .value = pimpl->last_draw_semaphore_value,
            },
            .stages = GAL::PipelineStage::ColorAttachmentOutput,
        };
        GAL::QueueSubmitConfig submit_config = {
            .wait_semaphores{&wait_config, 1},
            .signal_semaphores{&signal_config, 1},
            .command_buffers{&cmd_buffer, 1},
        };
        GAL::QueueSubmit(pimpl->queue, {&submit_config, 1});
    }

    GAL::SemaphoreState wait_state = {
        .semaphore = sem,
        .value = pimpl->last_draw_semaphore_value,
    };
    auto status = GAL::PresentImage(swc, img_idx, {&wait_state, 1}, nullptr);
    if (status != GAL::SwapchainStatus::Optimal) {
        resize_swapchain(status);
    }

    pimpl->frame_index = (pimpl->frame_index + 1) % pimpl->frame_count;
    pimpl->old_acquire_semaphore_value += pimpl->signal_cnt;
    pimpl->old_draw_semaphore_value += pimpl->signal_cnt;
}

Scene::~Scene() = default;
}
