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

GAPI::PipelineLayout createPipelineLayout(GAPI::Context ctx) {
    return GAPI::CreatePipelineLayout(ctx, {});
}

GAPI::Pipeline createPipeline(
    GAPI::Context ctx,
    GAPI::PipelineLayout layout,
    GAPI::ShaderModule vert_module,
    GAPI::ShaderModule frag_module,
    Format color_fmt
) {
    GAPI::GraphicsPipelineConfigurator gpc;

    gpc.SetLayout(layout);

    GAPI::ShaderStageInfo vert_stage = {
        .module = vert_module,
        .entry_point = "main",
    };
    VertexInputInfo vert_input = {};
    InputAssemblyInfo input_assembly = {
        .primitive_topology = PrimitiveTopology::TriangleList,
    };
    gpc.SetVertexShaderState(vert_stage, vert_input, {}, {}, input_assembly);

    RasterizationInfo rast = {
        .polygon_mode = PolygonMode::Fill,
        .cull_mode = CullMode::None,
        .line_width = 1.0f,
    };
    MultisampleInfo ms = {
        .sample_count = 1,
    };
    gpc.SetRasterizationState(rast, ms);

    GAPI::ShaderStageInfo frag_stage = {
        .module = frag_module,
        .entry_point = "main",
    };
    ColorBlendInfo blend = {};
    ColorAttachmentInfo att = {
        .format = color_fmt,
        .color_mask = {1, 1, 1, 1},
    };
    gpc.SetFragmentShaderState(frag_stage, blend, {&att, 1});
    gpc.FinishCurrent();

    GAPI::Pipeline pipeline = nullptr;
    GAPI::CreateGraphicsPipelines(ctx, nullptr, gpc.FinishAll(), &pipeline);

    return pipeline;
}

GAPI::ImageView createSwapchainImageView(
    GAPI::Context ctx, Format fmt, GAPI::Image img 
) {
    ImageViewConfig config = {
        .type = ImageViewType::D2,
        .format = fmt,
        .subresource_range = {
            .aspects = ImageAspect::Color,
            .first_mip_level = 0,
            .mip_level_count = 1,
            .first_array_layer = 0,
            .array_layer_count = 1,
        },
    };
    return GAPI::CreateImageView(ctx, img, config);
}

std::vector<GAPI::ImageView> createSwapchainImageViews(
    GAPI::Context ctx, Format fmt, GAPI::Swapchain swc
) {
    std::vector<GAPI::ImageView> views(GAPI::GetSwapchainImageCount(swc));
    for (size_t i = 0; i < views.size(); i++) {
        views[i] = createSwapchainImageView(ctx, fmt, GAPI::GetSwapchainImage(swc, i));
    }
    return views;
}

GAPI::CommandPool createCommandPool(GAPI::Context ctx, QueueFamily::ID qf) {
    CommandPoolConfig config = {
        .capabilities = {
            .transient = true,
            .reset_command_buffer = true,
        },
        .queue_family = qf,
    };
    return GAPI::CreateCommandPool(ctx, config);
}

std::vector<GAPI::CommandBuffer> createCommandBuffers(
    GAPI::Context ctx, GAPI::CommandPool pool, unsigned n
) {
    std::vector<GAPI::CommandBuffer> cmd_buffers(n);
    GAPI::AllocateCommandBuffers(ctx, pool, cmd_buffers);
    return cmd_buffers;
}

std::vector<GAPI::Fence> createFences(GAPI::Context ctx, unsigned n, bool signaled = true) {
    std::vector<GAPI::Fence> fences(n);
    std::ranges::generate(fences, [&] { return GAPI::CreateFence(ctx, { .signaled = signaled }); });
    return fences;
}

std::vector<GAPI::Semaphore> createSemaphores(GAPI::Context ctx, unsigned n) {
    std::vector<GAPI::Semaphore> semaphores(n);
    std::ranges::generate(semaphores, [&] { return GAPI::CreateSemaphore(ctx); });
    return semaphores;
}
}

struct Scene::Impl {
    GAPI::Context                       ctx;
    GAPI::Swapchain                     swapchain;
    QueueFamily::ID                     queue_family;
    GAPI::Queue                         queue;
    Format                              color_fmt;
    GAPI::Pipeline                      pipeline;
    std::vector<GAPI::ImageView>        image_views;
    unsigned                            frame_index;
    unsigned                            frame_count;
    GAPI::CommandPool                   command_pool;
    std::vector<GAPI::CommandBuffer>    command_buffers;
    std::vector<GAPI::Fence>            fences;
    std::vector<GAPI::Semaphore>        acquire_semaphores;
    std::vector<GAPI::Semaphore>        draw_semaphores;

    ~Impl() {
        GAPI::ContextWaitIdle(ctx);
        std::ranges::for_each(fences, [&] (auto f) { GAPI::DestroyFence(ctx, f); });
        std::ranges::for_each(acquire_semaphores, [&] (auto s) { GAPI::DestroySemaphore(ctx, s); });
        std::ranges::for_each(draw_semaphores, [&] (auto s) { GAPI::DestroySemaphore(ctx, s); });
        GAPI::FreeCommandBuffers(ctx, command_pool, command_buffers);
        GAPI::DestroyCommandPool(ctx, command_pool);
        std::ranges::for_each(image_views, [&] (auto v) { GAPI::DestroyImageView(ctx, v); });
        GAPI::DestroyPipeline(ctx, pipeline);
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
        auto vert_module = GAPI::CreateShaderModule(pimpl->ctx, { .code = vert_code } );
        auto frag_module = GAPI::CreateShaderModule(pimpl->ctx, { .code = frag_code } );
        auto layout = createPipelineLayout(pimpl->ctx);
        pimpl->pipeline = createPipeline(pimpl->ctx, layout, vert_module, frag_module, pimpl->color_fmt);
        GAPI::DestroyPipelineLayout(pimpl->ctx, layout);
        GAPI::DestroyShaderModule(pimpl->ctx, vert_module);
        GAPI::DestroyShaderModule(pimpl->ctx, frag_module);
    }
    {
        pimpl->image_views = createSwapchainImageViews(pimpl->ctx, pimpl->color_fmt, pimpl->swapchain);
    }
    pimpl->frame_index = 0;
    pimpl->frame_count = 2;
    pimpl->command_pool = createCommandPool(
        pimpl->ctx, pimpl->queue_family
    );
    pimpl->command_buffers = createCommandBuffers(
        pimpl->ctx, pimpl->command_pool, pimpl->frame_count
    );
    pimpl->fences = createFences(
        pimpl->ctx, pimpl->frame_count
    );
    pimpl->acquire_semaphores = createSemaphores(
        pimpl->ctx, pimpl->frame_count
    );
    pimpl->draw_semaphores = createSemaphores(
        pimpl->ctx, pimpl->frame_count
    );
}

void Scene::Draw() {
    auto ctx = pimpl->ctx;
    auto swc = pimpl->swapchain;
    auto idx = pimpl->frame_index;
    auto fence = pimpl->fences[idx];
    auto acquire_sem = pimpl->acquire_semaphores[idx];
    auto draw_sem = pimpl->draw_semaphores[idx];

    constexpr auto InfiniteTimeout = std::chrono::nanoseconds{UINT64_MAX};

    GAPI::WaitForFences(ctx, {&fence, 1}, true, InfiniteTimeout);
    GAPI::ResetFences(ctx, {&fence, 1});

    auto resize_swapchain = [&] (SwapchainStatus status) {
        if (status == SwapchainStatus::RequiresSlowResize) {
            GAPI::ContextWaitIdle(ctx);
            std::ranges::for_each(pimpl->command_buffers, [] (auto cmd) { GAPI::ResetCommandBuffer(cmd, CommandResources::Keep); });
            std::ranges::for_each(pimpl->image_views, [&] (auto v) { GAPI::DestroyImageView(ctx, v); });
            GAPI::ResizeSwapchain(swc);
            pimpl->image_views = createSwapchainImageViews(ctx, pimpl->color_fmt, swc);
        } else {
            GAPI::ResizeSwapchain(swc);
        }
    };

    auto img_idx = [&] {
    while(true) {
        auto [img_idx, status] = GAPI::AcquireImage(swc, acquire_sem);
        if (status != SwapchainStatus::Good) {
            resize_swapchain(status);
        } else {
            return img_idx;
        };
    }} ();

    auto [img_w, img_h] = GAPI::GetSwapchainSize(swc);
    auto img = GAPI::GetSwapchainImage(swc, img_idx);
    auto view = pimpl->image_views[img_idx];

    auto cmd_buffer = pimpl->command_buffers[idx];

    CommandBufferBeginConfig begin_config = {
        .usage = { .one_time_submit = true },
    };
    GAPI::BeginCommandBuffer(cmd_buffer, begin_config);

    {
        GAPI::ImageBarrier from_present = {
            .memory_barrier = {
                .src_stages = { .all_commands = true },
                .dst_stages = { .color_attachment_output = true },
                .dst_accesses = { .color_attachment_write = true },
            },
            .new_layout = ImageLayout::Attachment,
            .image = img,
            .subresource_range = {
                .aspects = ImageAspect::Color,
                .first_mip_level = 0,
                .mip_level_count = 1,
                .first_array_layer = 0,
                .array_layer_count = 1,
            },
        };
        GAPI::DependencyConfig config = {
            .image_barriers{&from_present, 1},
        };
        GAPI::CmdPipelineBarrier(cmd_buffer, config);
    }

    ClearValue clear_color = {0.2f, 0.2f, 0.8f, 1.0f};
    GAPI::RenderingAttachment color_attachment = {
        .view = view,
        .layout = ImageLayout::Attachment,
        .load_op = AttachmentLoadOp::Clear,
        .store_op = AttachmentStoreOp::Store,
        .clear_value = clear_color,
    };

    GAPI::RenderingConfig rendering_config = {
        .render_area = { .width = img_w, .height = img_h },
        .color_attachments{&color_attachment, 1},
    };

    GAPI::CmdBeginRendering(cmd_buffer, rendering_config);

    {
        GAPI::Viewport viewport = {
            .width = static_cast<float>(img_w),
            .height = static_cast<float>(img_h),
            .min_depth = 0.0f,
            .max_depth = 1.0f,
        };
        GAPI::Rect2D scissor = {
            .width = img_w,
            .height = img_h,
        };
        GAPI::CmdSetViewports(cmd_buffer, {&viewport, 1});
        GAPI::CmdSetScissors(cmd_buffer, {&scissor, 1});
    }

    GAPI::CmdBindGraphicsPipeline(cmd_buffer, pimpl->pipeline);
    DrawConfig draw_config = {
        .vertex_count = 3,
        .instance_count = 1,
    };
    GAPI::CmdDraw(cmd_buffer, draw_config);

    GAPI::CmdEndRendering(cmd_buffer);

    {
        GAPI::ImageBarrier to_present = {
            .memory_barrier = {
                .src_stages = { .color_attachment_output = true },
                .src_accesses = { .color_attachment_write = true },
            },
            .old_layout = ImageLayout::Attachment,
            .new_layout = ImageLayout::Present,
            .image = img,
            .subresource_range = {
                .aspects = ImageAspect::Color,
                .first_mip_level = 0,
                .mip_level_count = 1,
                .first_array_layer = 0,
                .array_layer_count = 1,
            },
        };
        GAPI::DependencyConfig config = {
            .image_barriers{&to_present, 1},
        };
        GAPI::CmdPipelineBarrier(cmd_buffer, config);
    }

    GAPI::EndCommandBuffer(cmd_buffer);

    {
        GAPI::SemaphoreSubmitConfig wait_config {
            acquire_sem, { .color_attachment_output = true }};
        GAPI::CommandBufferSubmitConfig cmd_config{cmd_buffer};
        GAPI::SemaphoreSubmitConfig signal_config {
            draw_sem, {}};
        GAPI::QueueSubmitConfig submit_config{
            {&wait_config, 1},
            {&cmd_config, 1},
            {&signal_config, 1},
        };
        GAPI::QueueSubmit(pimpl->queue, {&submit_config, 1}, fence);
    }

    auto status = GAPI::PresentImage(swc, img_idx, {&draw_sem, 1});
    if (status != SwapchainStatus::Good) {
        resize_swapchain(status);
    }

    pimpl->frame_index = (pimpl->frame_index + 1) % pimpl->frame_count;
}

Scene::~Scene() = default;
}
