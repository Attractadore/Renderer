#include "Common/Vector.hpp"
#include "Scene.hpp"

#include <filesystem>
#include <fstream>

#include <boost/container/static_vector.hpp>
#include <boost/container/small_vector.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace R1;
using boost::container::static_vector;

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

GAL::DescriptorSetLayout CreateDescriptorSetLayout(GAL::Context ctx) {
    std::array<GAL::DescriptorSetLayoutBinding, 2> bindings;
    bindings[0] = {
        .binding = GLSL::transform_ssbo_binding,
        .type = GAL::DescriptorType::DynamicStorageBuffer,
        .count = 1,
        .stages = GAL::ShaderStage::Vertex,
    };
    bindings[1] = {
        .binding = GLSL::global_ubo_binding,
        .type = GAL::DescriptorType::UniformBuffer,
        .count = 1,
        .stages = GAL::ShaderStage::Vertex,
    };
    return GAL::CreateDescriptorSetLayout(ctx, {
        .bindings = bindings,
    });
}

GAL::DescriptorPool CreateDescriptorPool(GAL::Context ctx, unsigned set_count) {
    std::array<GAL::DescriptorPoolSize, 2> pool_sizes;
    pool_sizes[0] = {
        .type = GAL::DescriptorType::DynamicStorageBuffer,
        .count = set_count,
    };
    pool_sizes[1] = {
        .type = GAL::DescriptorType::UniformBuffer,
        .count = set_count,
    };
    return GAL::CreateDescriptorPool(ctx, {
        .set_count = set_count,
        .pool_sizes = pool_sizes,
    });
}

void AllocateDescriptorSets(
    GAL::Context ctx, GAL::DescriptorPool pool,
    GAL::DescriptorSetLayout layout,
    std::span<GAL::DescriptorSet> sets
) {
    boost::container::small_vector<GAL::DescriptorSetLayout, 4> layouts(sets.size());
    std::ranges::fill(layouts, layout);
    auto res = GAL::AllocateDescriptorSets(ctx, pool, {
        .layouts = layouts,
    }, sets);
    assert(res == GAL::DescriptorSetAllocationResult::Success);
}

GAL::PipelineLayout createPipelineLayout(
    GAL::Context ctx, GAL::DescriptorSetLayout descriptor_set_layout
) {
    return GAL::CreatePipelineLayout(ctx, {
        .descriptor_set_layouts = {&descriptor_set_layout, 1},
    });
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
    GAL::VertexInputConfig vert_input = {
    };
    GAL::InputAssemblyConfig input_assembly = {
        .primitive_topology = GAL::PrimitiveTopology::TriangleList,
    };
    GAL::VertexInputBindingConfig binding = {
        .binding = 0,
        .stride = sizeof(glm::vec3),
        .input_rate = GAL::VertexInputRate::Vertex,
    };
    GAL::VertexInputAttributeConfig attribute = {
        .location = 0,
        .binding = 0,
        .format = GAL::Format::Float3,
        .offset = 0,
    };
    gpc.SetVertexShaderState(
        vert_stage, vert_input,
        {&binding, 1}, {&attribute, 1}, input_assembly);

    GAL::RasterizationConfig rast = {
        .polygon_mode = GAL::PolygonMode::Fill,
        .cull_mode = GAL::CullMode::None,
        .line_width = 1.0f,
    };
    GAL::MultisampleConfig ms = {
        .sample_count = 1,
    };
    gpc.SetRasterizationState(rast, ms);

    GAL::DepthTestConfig depth = {
        .compare_op = GAL::CompareOp::Greater,
        .enabled = true,
        .write_enabled = true,
    };
    GAL::DepthAttachmentConfig depth_attachment = {
        .format = GAL::Format::D32_FLOAT,
    };
    gpc.SetDepthTestState(depth, depth_attachment);

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

GAL::Image CreateDepthBuffer(
    GAL::Context ctx,
    unsigned width,
    unsigned height
) {
    return CreateImage(
        ctx,
        width, height,
        GAL::Format::D32_FLOAT,
        GAL::ImageUsage::DepthAttachment);
}

GAL::ImageView CreateDepthBufferView(GAL::Context ctx, GAL::Image depth_buffer) {
    GAL::ImageViewConfig config = {
        .type = GAL::ImageViewType::D2,
        .format = GAL::Format::D32_FLOAT,
        .components = {
            .r = GAL::ImageComponentSwizzle::Identity,
        },
        .subresource_range = {
            .aspects = GAL::ImageAspect::Depth,
            .mip_level_count = 1,
            .array_layer_count = 1,
        },
    };
    return GAL::CreateImageView(ctx, depth_buffer, config);
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
    GAL::Image                      depth_buffer;
    GAL::ImageView                  depth_buffer_view;
    unsigned                        image_width = 0;
    unsigned                        image_height = 0;
    unsigned                        frame_index = 0;
    GAL::DescriptorSetLayout        descriptor_set_layout;
    GAL::DescriptorPool             descriptor_pool;
    std::vector<GAL::DescriptorSet> descriptor_sets;
    GAPI::HBuffer                   uniform_ring_buffer;
    GLSL::GlobalUBO*                uniform_ring_buffer_data;
    GAL::PipelineLayout             pipeline_layout;
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
        GAL::DestroyPipelineLayout(ctx, pipeline_layout);
        GAL::DestroyDescriptorPool(ctx, descriptor_pool);
        GAL::DestroyDescriptorSetLayout(ctx, descriptor_set_layout);
        DestroyImages(ctx, images);
        DestroyImageViews(ctx, image_views);
        GAL::DestroyImage(ctx, depth_buffer);
        GAL::DestroyImageView(ctx, depth_buffer_view);
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
        pimpl->descriptor_set_layout = CreateDescriptorSetLayout(pimpl->ctx);
        pimpl->pipeline_layout = createPipelineLayout(pimpl->ctx, pimpl->descriptor_set_layout);
        pimpl->pipeline = createPipeline(pimpl->ctx, pimpl->pipeline_layout, vert_module, frag_module, pimpl->image_fmt);
        GAL::DestroyShaderModule(pimpl->ctx, vert_module);
        GAL::DestroyShaderModule(pimpl->ctx, frag_module);
    }
    pimpl->command_pool = createCommandPool(
        pimpl->ctx, pimpl->queue_family
    );
    pimpl->semaphore = GAL::CreateSemaphore(pimpl->ctx, {.initial_value = pimpl->last_semaphore_value});
    m_upload_semaphore = GAPI::HSemaphore{pimpl->ctx,
        GAL::CreateSemaphore(pimpl->ctx,
            {.initial_value = m_last_upload_time})};
    m_upload_command_pool = GAPI::HCommandPool{pimpl->ctx,
        GAL::CreateCommandPool(pimpl->ctx, {
            .flags = GAL::CommandPoolConfigOption::Transient,
            .queue_family = pimpl->queue_family,
        })};
}

Scene::~R1Scene() {
    GAL::ContextWaitIdle(pimpl->ctx);
    m_streaming_buffers.clear();
    FlushUploadQueue();
    PushDeleteQueue();
    FlushDeleteQueue();
    assert(m_meshes.empty());
    assert(m_mesh_instances.empty());
    assert(m_upload_queue.empty());
    assert(m_buffer_delete_queue.empty());
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

    GAL::DestroyImage(ctx, pimpl->depth_buffer);
    pimpl->depth_buffer =
        CreateDepthBuffer(ctx, pimpl->image_width, pimpl->image_height);
    GAL::DestroyImageView(ctx, pimpl->depth_buffer_view);
    pimpl->depth_buffer_view =
        CreateDepthBufferView(ctx, pimpl->depth_buffer);

    GAL::FreeCommandBuffers(ctx, pimpl->command_pool, pimpl->command_buffers);
    pimpl->command_buffers.resize(pimpl->images.size());
    GAL::AllocateCommandBuffers(ctx, pimpl->command_pool, pimpl->command_buffers);

    GAL::DestroyDescriptorPool(ctx, pimpl->descriptor_pool);
    pimpl->descriptor_pool = CreateDescriptorPool(ctx, pimpl->images.size());
    pimpl->descriptor_sets.resize(pimpl->images.size());
    AllocateDescriptorSets(ctx, pimpl->descriptor_pool,
        pimpl->descriptor_set_layout, pimpl->descriptor_sets);

    pimpl->uniform_ring_buffer = GAPI::HBuffer{pimpl->ctx, GAL::CreateBuffer(pimpl->ctx, {
        .size = sizeof(GLSL::GlobalUBO) * pimpl->images.size(),
        .usage =
            GAL::BufferUsage::Uniform,
        .memory_usage = GAL::BufferMemoryUsage::Streaming,
    })};
    pimpl->uniform_ring_buffer_data = reinterpret_cast<GLSL::GlobalUBO*>(
        GAL::GetBufferPointer(pimpl->ctx, pimpl->uniform_ring_buffer.get()));

    while(m_streaming_buffers.size() > count) {
        m_streaming_buffers.pop_back();
    }
    while(m_streaming_buffers.size() < count) {
        m_streaming_buffers.emplace_back(StreamingBufferAllocator<std::byte>(
            pimpl->ctx, &m_buffer_delete_queue));
    }

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
    auto descriptor_set = pimpl->descriptor_sets[idx];
    auto cmd_buffer = pimpl->command_buffers[idx];
    auto img = pimpl->images[idx];
    auto img_view = pimpl->image_views[idx];
    auto img_w = pimpl->image_width;
    auto img_h = pimpl->image_height;

    using boost::container::static_vector;

    static_vector<GAL::SemaphoreSubmitConfig, 1> upload_signal_submits;
    static_vector<GAL::CommandBuffer, 1> upload_cmd_submits;
    static_vector<GAL::SemaphoreSubmitConfig, 2> draw_wait_submits;
    static_vector<GAL::SemaphoreSubmitConfig, 1> draw_signal_submits;
    static_vector<GAL::CommandBuffer, 1> draw_cmd_submits;
    static_vector<GAL::QueueSubmitConfig, 2> submits;

    if (not m_mesh_staging_infos.empty()) {
        PushUploadQueue();
        upload_signal_submits.emplace_back() = {
            .state = {
                .semaphore = m_upload_semaphore.get(),
                .value = m_last_upload_time,
            },
            .stages = GAL::PipelineStage::Copy,
        };
        upload_cmd_submits.emplace_back() =
            m_upload_queue.back().cmd_buffer;
        submits.emplace_back() = {
            .signal_semaphores = upload_signal_submits,
            .command_buffers = upload_cmd_submits,
        };
    }
    FlushUploadQueue();

    PushDeleteQueue();
    FlushDeleteQueue();

    {
        GAL::SemaphoreState wait_state = {
            .semaphore = sem,
            .value = pimpl->draw_timepoint.oldest_value,
        };
        GAL::WaitForSemaphores(ctx, {&wait_state, 1}, true, pimpl->InfiniteTimeout);
    }

    auto& ubo = pimpl->uniform_ring_buffer_data[idx];
    { auto aspect_ratio = static_cast<float>(img_w) / img_h;
    // Setup projection matrix for reverse-Z
    auto fov = glm::min(m_camera.fov / aspect_ratio, glm::radians(170.0f));
    auto proj = glm::perspectiveZO(fov, aspect_ratio, m_camera.far, m_camera.near);
    auto view = glm::lookAt(m_camera.position, m_camera.position + m_camera.direction, m_camera.up);
    GLSL::GlobalUBO staging = {
        .proj_view = proj * view,
    };
    ubo = staging; }

    auto index_view = ranges::views::enumerate(
        m_mesh_instances.values() |
        ranges::views::transform(
        [] (const MeshInstanceDesc& desc) {
            return desc.mesh;
        })
    );
    auto sorted_mesh_instance_data = vec_from_range(index_view);
    std::ranges::sort(sorted_mesh_instance_data,
        [] (const auto& l, const auto& r) {
            return l.second < r.second;
        });

    auto& streaming_buffer = m_streaming_buffers[idx];
    streaming_buffer.resize(
        sizeof(glm::mat4) * sorted_mesh_instance_data.size());
    { auto ptr = reinterpret_cast<glm::mat4*>(streaming_buffer.data());
    for (auto&& [idx, mesh]: sorted_mesh_instance_data) {
        *(ptr++) = m_mesh_instances.values()[idx].transform;
    } }

    { GAL::DescriptorBufferConfig ssbo_config = {
        .buffer = streaming_buffer.GetBackingBuffer(),
        .size = streaming_buffer.size(),
    };
    GAL::DescriptorBufferConfig ubo_config = {
        .buffer = pimpl->uniform_ring_buffer.get(),
        .offset = sizeof(GLSL::GlobalUBO) * idx,
        .size = sizeof(GLSL::GlobalUBO),
    };
    std::array<GAL::DescriptorSetWriteConfig, 2> writes;
    writes[0] = {
        .set = descriptor_set,
        .binding = GLSL::transform_ssbo_binding,
        .type = GAL::DescriptorType::DynamicStorageBuffer,
        .buffer_configs = {&ssbo_config, 1},
    };
    writes[1] = {
        .set = descriptor_set,
        .binding = GLSL::global_ubo_binding,
        .type = GAL::DescriptorType::UniformBuffer,
        .buffer_configs = {&ubo_config, 1},
    };
    GAL::UpdateDescriptorSets(ctx, writes, {}); }

    GAL::CommandBufferBeginConfig begin_config = {
        .usage = GAL::CommandBufferUsage::OneTimeSubmit,
    };
    GAL::BeginCommandBuffer(ctx, cmd_buffer, begin_config);

    {
        std::array<GAL::ImageBarrier, 2> image_barriers;
        image_barriers[0] = {
            .memory_barrier = {
                .src_stages = GAL::PipelineStage::ColorAttachmentOutput,
                .dst_stages = GAL::PipelineStage::ColorAttachmentOutput,
                .dst_accesses = GAL::MemoryAccess::ColorAttachmentWrite,
            },
            .new_layout = GAL::ImageLayout::Attachment,
            .image = img,
            .subresource_range = {
                .aspects = GAL::ImageAspect::Color,
                .mip_level_count = 1,
                .array_layer_count = 1,
            },
        };
        image_barriers[1] = {
            .memory_barrier = {
                .src_stages =
                    GAL::PipelineStage::EarlyFragmentTests |
                    GAL::PipelineStage::LateFragmentTests,
                .dst_stages =
                    GAL::PipelineStage::EarlyFragmentTests |
                    GAL::PipelineStage::LateFragmentTests,
                .dst_accesses =
                    GAL::MemoryAccess::DepthAttachmentRead |
                    GAL::MemoryAccess::DepthAttachmentWrite,
            },
            .new_layout = GAL::ImageLayout::Attachment,
            .image = pimpl->depth_buffer,
            .subresource_range = {
                .aspects = GAL::ImageAspect::Depth,
                .mip_level_count = 1,
                .array_layer_count = 1,
            },
        };
        GAL::DependencyConfig config = {
            .image_barriers = image_barriers,
        };
        GAL::CmdPipelineBarrier(ctx, cmd_buffer, config);
    }

    { GAL::ClearValue clear_color = {0.0f, 0.0f, 0.0f, 1.0f};
    GAL::RenderingAttachment color_attachment = {
        .view = img_view,
        .layout = GAL::ImageLayout::Attachment,
        .load_op = GAL::AttachmentLoadOp::Clear,
        .store_op = GAL::AttachmentStoreOp::Store,
        .clear_value = clear_color,
    };
    GAL::ClearValue clear_depth = { .depth = 0.0f };
    GAL::RenderingAttachment depth_attachment = {
        .view = pimpl->depth_buffer_view,
        .layout = GAL::ImageLayout::Attachment,
        .load_op = GAL::AttachmentLoadOp::Clear,
        .store_op = GAL::AttachmentStoreOp::DontCare,
        .clear_value = clear_depth,
    };
    GAL::RenderingConfig rendering_config = {
        .render_area = { .width = img_w, .height = img_h },
        .color_attachments = {&color_attachment, 1},
        .depth_attachment = depth_attachment,
    };
    GAL::CmdBeginRendering(ctx, cmd_buffer, rendering_config); }

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

    auto mesh_instances_same_meshes = sorted_mesh_instance_data |
        ranges::views::transform([] (const auto& v) {
            return v.second;
        }) |
        ranges::views::chunk_by(std::ranges::equal_to{});

    { auto ptr = streaming_buffer.data();
    for (auto&& mesh_instances_same_mesh: mesh_instances_same_meshes) {
        auto& mesh = m_meshes[mesh_instances_same_mesh.front()];
        auto buffer = mesh.buffer;
        size_t offset = 0;
        GAL::CmdBindVertexBuffers(ctx, cmd_buffer, {
            .buffers = {&buffer, 1},
            .offsets = {&offset, 1},
        });
        size_t buffer_size = mesh.vertex_count * sizeof(glm::vec3);
        unsigned dynamic_offset = ptr - streaming_buffer.data();
        ptr += buffer_size;
        GAL::CmdBindGraphicsPipelineDescriptorSets(ctx, cmd_buffer, {
            .layout = pimpl->pipeline_layout,
            .sets = {&descriptor_set, 1},
            .dynamic_offsets = {&dynamic_offset, 1},
        });
        GAL::CmdDraw(ctx, cmd_buffer, {
            .vertex_count = mesh.vertex_count,
            .instance_count =
                static_cast<unsigned>(mesh_instances_same_mesh.size()),
        });
    } }

    GAL::CmdEndRendering(ctx, cmd_buffer);

    GAL::EndCommandBuffer(ctx, cmd_buffer);

    {
        pimpl->draw_timepoint.new_value = ++pimpl->last_semaphore_value;
        draw_wait_submits.emplace_back() = {
            .state = {
                .semaphore = sem,
                .value = pimpl->external_timepoint.oldest_value,
            },
            .stages = GAL::PipelineStage::ColorAttachmentOutput,
        };
        if (not upload_cmd_submits.empty()) {
            draw_wait_submits.emplace_back() = {
                .state = {
                    .semaphore = m_upload_semaphore.get(),
                    .value = m_last_upload_time,
                },
                .stages = GAL::PipelineStage::VertexAttributeInput,
            };
        }
        draw_signal_submits.emplace_back() = {
            .state = {
                .semaphore = sem,
                .value = pimpl->draw_timepoint.new_value,
            },
            .stages = GAL::PipelineStage::ColorAttachmentOutput,
        };
        draw_cmd_submits.emplace_back(cmd_buffer);
        submits.emplace_back() = {
            .wait_semaphores = draw_wait_submits,
            .signal_semaphores = draw_signal_submits,
            .command_buffers = draw_cmd_submits,
        };
        pimpl->external_timepoint.new_value = ++pimpl->last_semaphore_value;
    }
    GAL::QueueSubmit(ctx, pimpl->queue, submits);

    pimpl->frame_index = (idx + 1) % pimpl->images.size();
    pimpl->draw_timepoint.oldest_value += pimpl->signal_cnt;
    pimpl->external_timepoint.oldest_value += pimpl->signal_cnt;
    m_buffer_delete_queue.last_used = pimpl->draw_timepoint.new_value;

    return {
        .semaphore = sem,
        .wait_value = pimpl->draw_timepoint.new_value,
        .signal_value = pimpl->external_timepoint.new_value,
    };
}

MeshID Scene::CreateMesh(const MeshConfig& config) {
    auto ctx = pimpl->ctx;
    auto ptr =
        reinterpret_cast<const std::byte*>(config.vertices.data());
    auto sz = config.vertices.size_bytes();
    unsigned vert_cnt = config.vertices.size() / 3;

    m_staging_storage.insert(m_staging_storage.end(), ptr, ptr + sz);

    auto&& [key, mesh] = m_meshes.emplace();
    mesh = {
        .vertex_count = vert_cnt,
    };
    m_mesh_staging_infos.emplace_back() = {
        .key = key,
        .vertex_count = vert_cnt,
    };

    auto id = std::bit_cast<MeshID>(key);

    return id;
}

void Scene::DestroyMesh(MeshID mesh) {
    m_mesh_delete_infos.push_back(mesh);
}

MeshInstanceID Scene::CreateMeshInstance(const MeshInstanceConfig& config) {
    auto&& [key, ref] = m_mesh_instances.emplace();
    ref.transform = config.transform;
    ref.mesh = std::bit_cast<MeshKey>(config.mesh);
    return std::bit_cast<MeshInstanceID>(key);
}

void Scene::DestroyMeshInstance(MeshInstanceID mesh_instance) {
    auto key = std::bit_cast<MeshInstanceKey>(mesh_instance);
    m_mesh_instances.erase(key);
}

const glm::mat4& Scene::GetMeshInstanceTransform(R1::MeshInstanceID mesh_instance) const noexcept {
    auto key = std::bit_cast<MeshInstanceKey>(mesh_instance);
    return m_mesh_instances[key].transform;
}

glm::mat4& Scene::GetMeshInstanceTransform(R1::MeshInstanceID mesh_instance) noexcept {
    auto key = std::bit_cast<MeshInstanceKey>(mesh_instance);
    return m_mesh_instances[key].transform;
}

void Scene::PushUploadQueue() {
    auto ctx = pimpl->ctx;
    auto upload_time = ++m_last_upload_time;

    auto staging_buffer_sz = m_staging_storage.size();

    auto staging_buffer = GAL::CreateBuffer(ctx, {
        .size = staging_buffer_sz,
        .usage = GAL::BufferUsage::TransferSRC,
        .memory_usage = GAL::BufferMemoryUsage::Staging,
    });
    auto mapped_data =
         reinterpret_cast<std::byte*>(GAL::GetBufferPointer(ctx, staging_buffer));
    std::ranges::copy(m_staging_storage, mapped_data);
    m_staging_storage.clear();
    GAL::FlushBufferRange(ctx, staging_buffer, 0, staging_buffer_sz);

    GAL::CommandBuffer cmd_buffer;
    GAL::AllocateCommandBuffers(ctx, m_upload_command_pool.get(), {&cmd_buffer, 1});

    GAL::BeginCommandBuffer(ctx, cmd_buffer,
        {.usage = GAL::CommandBufferUsage::OneTimeSubmit});

    size_t offset = 0;
    for (const auto& config: m_mesh_staging_infos) {
        auto size = config.vertex_count * sizeof(float[3]);
        auto buffer = GAL::CreateBuffer(ctx, {
            .size = size,
            .usage =
                GAL::BufferUsage::TransferDST |
                GAL::BufferUsage::Vertex,
            .memory_usage = GAL::BufferMemoryUsage::Device,
        });

        GAL::BufferCopyRegion region = {
            .src_offset = offset,
            .size = size,
        };

        GAL::CmdCopyBuffer(ctx, cmd_buffer, {
            .src = staging_buffer,
            .dst = buffer,
            .regions = {&region, 1},
        });

        auto& mesh = m_meshes[config.key];
        mesh.buffer = buffer;
        mesh.upload_time = upload_time;

        offset += size;
    }
    m_mesh_staging_infos.clear();

    GAL::EndCommandBuffer(ctx, cmd_buffer);

    m_upload_queue.emplace() = {
        .staging_buffer = staging_buffer,
        .cmd_buffer = cmd_buffer,
        .upload_time = upload_time,
    };
}

void Scene::FlushUploadQueue() {
    auto ctx = pimpl->ctx;
    auto last_uploaded =
        GAL::GetSemaphorePayloadValue(ctx, m_upload_semaphore.get());
    while(not m_upload_queue.empty()) {
        auto& u = m_upload_queue.front();
        if (u.upload_time > last_uploaded) {
            break;
        }
        GAL::DestroyBuffer(ctx, u.staging_buffer);
        GAL::FreeCommandBuffers(
            ctx, m_upload_command_pool.get(), {&u.cmd_buffer, 1});
        m_upload_queue.pop();
    }
}

void Scene::PushDeleteQueue() {
    for (auto mesh: m_mesh_delete_infos) {
        auto key = std::bit_cast<MeshKey>(mesh);
        auto it = m_meshes.access(key);
        m_buffer_delete_queue.emplace() = {
            .buffer = it->second.buffer,
            .upload_time = it->second.upload_time,
            .last_used = pimpl->draw_timepoint.new_value,
        };
        m_meshes.erase(it);
    }
    m_mesh_delete_infos.clear();
}

void Scene::FlushDeleteQueue() {
    auto ctx = pimpl->ctx;
    auto last_uploaded =
        GAL::GetSemaphorePayloadValue(ctx, m_upload_semaphore.get());
    auto last_drawn =
        GAL::GetSemaphorePayloadValue(ctx, pimpl->semaphore);
    while (not m_buffer_delete_queue.empty()) {
        auto& d = m_buffer_delete_queue.front();
        if (d.last_used > last_drawn or d.upload_time > last_uploaded) {
            break;
        }
        GAL::DestroyBuffer(ctx, d.buffer);
        m_buffer_delete_queue.pop();
    }
}
