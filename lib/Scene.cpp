#include "Scene.hpp"

#include <algorithm>
#include <filesystem>
#include <fstream>

using namespace R1;

namespace R1 {
namespace {
GAL::Format SelectColorFormat(GAL::Context ctx) {
    return GAL::Format::RGBA8_UNORM;
}

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
    GAL::Format image_fmt
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
        .format = image_fmt,
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

GAL::Image CreateImage(
    GAL::Context ctx,
    unsigned width,
    unsigned height,
    GAL::Format fmt,
    GAL::ImageUsageFlags usage
) {
    GAL::ImageConfig config = {
        .type = GAL::ImageType::D2,
        .format = fmt,
        .width = width,
        .height = height,
        .depth = 1,
        .mip_level_count = 1,
        .array_layer_count = 1,
        .sample_count = 1,
        .usage = usage,
        .initial_layout = GAL::ImageLayout::Undefined,
        .memory_usage = GAL::ImageMemoryUsage::Dedicated,
    };
    return GAL::CreateImage(ctx, config);
}

std::vector<GAL::Image> CreateImages(
    GAL::Context ctx,
    unsigned count,
    unsigned width,
    unsigned height,
    GAL::Format fmt,
    GAL::ImageUsageFlags usage
) {
    std::vector<GAL::Image> images(count);
    std::ranges::generate(images,
        [&] { return CreateImage(ctx, width, height, fmt, usage); });
    return images;
}

GAL::ImageView CreateImageView(
    GAL::Context ctx,
    GAL::Image img,
    GAL::Format fmt
) {
    GAL::ImageViewConfig config = {
        .type = GAL::ImageViewType::D2,
        .format = fmt,
        .components = {
            .r = GAL::ImageComponentSwizzle::Identity,
            .g = GAL::ImageComponentSwizzle::Identity,
            .b = GAL::ImageComponentSwizzle::Identity,
            .a = GAL::ImageComponentSwizzle::Identity,
        },
        .subresource_range = {
            .aspects = GAL::ImageAspect::Color,
            .mip_level_count = 1,
            .array_layer_count = 1,
        },
    };
    return GAL::CreateImageView(ctx, img, config);
}

template<std::ranges::input_range R>
    requires std::same_as<GAL::Image, std::ranges::range_value_t<R>>
std::vector<GAL::ImageView> CreateImageViews(
    GAL::Context ctx,
    R&& images,
    GAL::Format fmt
) {
    std::vector<GAL::ImageView> views(images.size());
    std::ranges::transform(images, views.begin(),
        [&] (GAL::Image img) { return CreateImageView(ctx, img, fmt); });
    return views;
}

template<std::ranges::input_range R>
    requires std::same_as<GAL::Image, std::ranges::range_value_t<R>>
void DestroyImages(GAL::Context ctx, R&& images) {
    std::ranges::for_each(images, [&] (GAL::Image img) { GAL::DestroyImage(ctx, img); });
}

template<std::ranges::input_range R>
    requires std::same_as<GAL::ImageView, std::ranges::range_value_t<R>>
void DestroyImageViews(GAL::Context ctx, R&& views) {
    std::ranges::for_each(views, [&] (GAL::ImageView v) { GAL::DestroyImageView(ctx, v); });
}
}
}

struct Scene::Impl {
    GAL::Context                    ctx;
    GAL::QueueFamily::ID            queue_family;
    GAL::Queue                      queue;
    GAL::Format                     image_fmt;
    static constexpr GAL::ImageUsageFlags
                                    required_image_usage_flags = GAL::ImageUsage::ColorAttachment;
    GAL::ImageUsageFlags            image_usage_flags;
    std::vector<GAL::Image>         images;
    std::vector<GAL::ImageView>     image_views;
    unsigned                        image_width = 0;
    unsigned                        image_height = 0;
    unsigned                        frame_index = 0;
    GAL::Pipeline                   pipeline;
    GAL::CommandPool                command_pool;
    std::vector<GAL::CommandBuffer> command_buffers;

    GAL::Semaphore                  semaphore;

    struct Timepoint {
        GAL::SemaphorePayload       oldest_value, new_value;
    };
    Timepoint                       draw_timepoint;
    Timepoint                       external_timepoint;
    size_t                          signal_cnt = std::size({
                                        draw_timepoint,
                                        external_timepoint});
    GAL::SemaphorePayload           last_semaphore_value = signal_cnt - 1;
    static constexpr auto           InfiniteTimeout = std::chrono::nanoseconds{UINT64_MAX};

    ~Impl() {
        GAL::ContextWaitIdle(ctx);
        GAL::DestroySemaphore(ctx, semaphore);
        GAL::FreeCommandBuffers(ctx, command_pool, command_buffers);
        GAL::DestroyCommandPool(ctx, command_pool);
        GAL::DestroyPipeline(ctx, pipeline);
        DestroyImages(ctx, images);
        DestroyImageViews(ctx, image_views);
    }
};

Scene::R1Scene(Context& ctx):
    pimpl{std::make_unique<Impl>()}
{
    pimpl->ctx = ctx.get().get();
    pimpl->queue_family = ctx.get().GetGraphicsQueueFamily();
    pimpl->queue = ctx.get().GetGraphicsQueue();
    pimpl->image_fmt = SelectColorFormat(pimpl->ctx);
    {
        auto vert_code = loadShader("vert.spv");
        auto frag_code = loadShader("frag.spv");
        auto vert_module = GAL::CreateShaderModule(pimpl->ctx, { .code = vert_code } );
        auto frag_module = GAL::CreateShaderModule(pimpl->ctx, { .code = frag_code } );
        auto layout = createPipelineLayout(pimpl->ctx);
        pimpl->pipeline = createPipeline(pimpl->ctx, layout, vert_module, frag_module, pimpl->image_fmt);
        GAL::DestroyPipelineLayout(pimpl->ctx, layout);
        GAL::DestroyShaderModule(pimpl->ctx, vert_module);
        GAL::DestroyShaderModule(pimpl->ctx, frag_module);
    }
    pimpl->command_pool = createCommandPool(
        pimpl->ctx, pimpl->queue_family
    );
    pimpl->semaphore = GAL::CreateSemaphore(pimpl->ctx, {.initial_value = pimpl->last_semaphore_value});
}

void Scene::ConfigOutputImages(
    unsigned width, unsigned height, unsigned count,
    GAL::ImageUsageFlags image_usage_flags
) {
    auto ctx = pimpl->ctx;
    GAL::ContextWaitIdle(ctx);

    DestroyImages(ctx, pimpl->images);
    pimpl->image_width = width;
    pimpl->image_height = height;
    pimpl->image_usage_flags = image_usage_flags | pimpl->required_image_usage_flags;
    pimpl->images = CreateImages(ctx,
        count, pimpl->image_width, pimpl->image_height,
        pimpl->image_fmt, pimpl->image_usage_flags);

    DestroyImageViews(ctx, pimpl->image_views);
    pimpl->image_views = CreateImageViews(ctx, pimpl->images, pimpl->image_fmt);

    GAL::FreeCommandBuffers(ctx, pimpl->command_pool, pimpl->command_buffers);
    pimpl->command_buffers.resize(pimpl->images.size());
    GAL::AllocateCommandBuffers(ctx, pimpl->command_pool, pimpl->command_buffers);

    pimpl->frame_index = 0;

    // Pretend that count virtual frames have been drawn.
    // Naturally, all of them are ready, so adjust semaphores
    // to reflect that.
    auto pipeline_time = pimpl->signal_cnt * pimpl->images.size();
    auto oldest_frame_time_offset = pipeline_time - pimpl->signal_cnt;
    pimpl->last_semaphore_value += pipeline_time;
    pimpl->external_timepoint.new_value = pimpl->last_semaphore_value;
    pimpl->external_timepoint.oldest_value = pimpl->external_timepoint.new_value - oldest_frame_time_offset;
    pimpl->draw_timepoint.new_value     = pimpl->external_timepoint.new_value - 1;
    pimpl->draw_timepoint.oldest_value     = pimpl->draw_timepoint.new_value - oldest_frame_time_offset;
    auto sem = pimpl->semaphore;
    GAL::SignalSemaphore(ctx, { .semaphore = sem, .value = pimpl->last_semaphore_value });
}

std::tuple<unsigned, unsigned> Scene::GetOutputImageSize() const noexcept {
    return {pimpl->image_width, pimpl->image_height};
}

GAL::Format Scene::GetOutputImageFormat() const noexcept {
    return pimpl->image_fmt;
}

size_t Scene::GetOutputImageCount() const noexcept {
    return pimpl->images.size();
}

GAL::Image Scene::GetOutputImage(size_t idx) const noexcept {
    return pimpl->images[idx];
}

size_t Scene::GetCurrentOutputImage() const noexcept {
    return pimpl->frame_index;
}

GAL::ImageLayout Scene::GetOutputImageStartLayout() const noexcept {
    return GAL::ImageLayout::Undefined;
}

GAL::ImageLayout Scene::GetOutputImageEndLayout() const noexcept {
    return GAL::ImageLayout::Attachment;
}

ScenePresentInfo Scene::Draw() {
    auto ctx = pimpl->ctx;
    auto idx = pimpl->frame_index;
    auto sem = pimpl->semaphore;

    {
        GAL::SemaphoreState wait_state = {
            .semaphore = sem,
            .value = pimpl->draw_timepoint.oldest_value,
        };
        GAL::WaitForSemaphores(ctx, {&wait_state, 1}, true, pimpl->InfiniteTimeout);
    }

    auto cmd_buffer = pimpl->command_buffers[idx];
    auto img = pimpl->images[idx];
    auto view = pimpl->image_views[idx];
    auto img_w = pimpl->image_width;
    auto img_h = pimpl->image_height;

    GAL::CommandBufferBeginConfig begin_config = {
        .usage = GAL::CommandBufferUsage::OneTimeSubmit,
    };
    GAL::BeginCommandBuffer(ctx, cmd_buffer, begin_config);

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
        GAL::CmdPipelineBarrier(ctx, cmd_buffer, config);
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

    GAL::CmdBeginRendering(ctx, cmd_buffer, rendering_config);

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
        GAL::CmdSetViewports(ctx, cmd_buffer, {&viewport, 1});
        GAL::CmdSetScissors(ctx, cmd_buffer, {&scissor, 1});
    }

    GAL::CmdBindGraphicsPipeline(ctx, cmd_buffer, pimpl->pipeline);
    GAL::DrawConfig draw_config = {
        .vertex_count = 3,
        .instance_count = 1,
    };
    GAL::CmdDraw(ctx, cmd_buffer, draw_config);

    GAL::CmdEndRendering(ctx, cmd_buffer);

    GAL::EndCommandBuffer(ctx, cmd_buffer);

    {
        pimpl->draw_timepoint.new_value = ++pimpl->last_semaphore_value;
        GAL::SemaphoreSubmitConfig wait_config = {
            .state = {
                .semaphore = sem,
                .value = pimpl->external_timepoint.oldest_value,
            },
            .stages = GAL::PipelineStage::ColorAttachmentOutput,
        };
        GAL::SemaphoreSubmitConfig signal_config = {
            .state = {
                .semaphore = sem,
                .value = pimpl->draw_timepoint.new_value,
            },
            .stages = GAL::PipelineStage::ColorAttachmentOutput,
        };
        GAL::QueueSubmitConfig submit_config = {
            .wait_semaphores{&wait_config, 1},
            .signal_semaphores{&signal_config, 1},
            .command_buffers{&cmd_buffer, 1},
        };
        GAL::QueueSubmit(ctx, pimpl->queue, {&submit_config, 1});
        pimpl->external_timepoint.new_value = ++pimpl->last_semaphore_value;
    }

    pimpl->frame_index = (idx + 1) % pimpl->images.size();
    pimpl->draw_timepoint.oldest_value += pimpl->signal_cnt;
    pimpl->external_timepoint.oldest_value += pimpl->signal_cnt;

    return {
        .semaphore = sem,
        .wait_value = pimpl->draw_timepoint.new_value,
        .signal_value = pimpl->external_timepoint.new_value,
    };
}

Scene::~R1Scene() = default;
