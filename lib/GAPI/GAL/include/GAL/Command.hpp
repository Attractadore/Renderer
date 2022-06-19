#pragma once
#if GAL_USE_VULKAN
#include "VulkanCommand.hpp"
#endif

#include "Buffer.hpp"
#include "Pipeline.hpp"
#include "Image.hpp"

namespace R1::GAL {
namespace Detail {
template<typename E>
concept IsCommandPoolConfigOption = requires(E e) {
    E::Transient;
    E::AllowCommandBufferReset;
};

template<typename E>
concept IsCommandResources = requires(E e) {
    E::Keep;
    E::Release;
};

template<typename E>
concept IsCommandBufferUsage = requires(E e) {
    E::OneTimeSubmit;
    E::Simultaneous;
};

template<typename E>
concept IsPipelineStage = requires(E e) {
    E::DrawIndirect;

    E::IndexInput;
    E::VertexAttributeInput;
    E::VertexInput;

    E::VertexShader;
    E::TesselationControlShader;
    E::TesselationEvaluationShader;
    E::GeometryShader;
    E::PreRasterizationShaders;

    E::FragmentShader;
    E::EarlyFragmentTests;
    E::LateFragmentTests;
    E::ColorAttachmentOutput;

    E::AllGraphics;

    E::ComputeShader;
    E::AllCompute;

    E::Copy;
    E::Resolve;
    E::Blit;
    E::Clear;
    E::AllTransfer;

    E::AllCommands;
};

template<typename E>
concept IsMemoryAccess = requires(E e) {
    E::IndirectCommandRead;

    E::IndexRead;
    E::VertexAttributeRead;
    E::VertexRead;

    E::ShaderUniformRead;
    E::ShaderSampledRead;
    E::ShaderStorageRead;
    E::ShaderRead;

    E::ShaderStorageWrite;
    E::ShaderWrite;

    E::ColorAttachmentRead;
    E::DepthAttachmentRead;
    E::StencilAttachmentRead;
    E::DepthStencilAttachmentRead;
    E::AttachmentRead;

    E::ColorAttachmentWrite;
    E::DepthAttachmentWrite;
    E::StencilAttachmentWrite;
    E::DepthStencilAttachmentWrite;
    E::AttachmentWrite;

    E::TransferRead;
    E::TransferWrite;

    E::MemoryRead;
    E::MemoryWrite;
};

template<typename E>
concept IsResolveMode = requires(E e) {
    E::None;
    E::SampleZero;
    E::Average;
    E::Min;
    E::Max;
};

template<typename E>
concept IsAttachmentLoadOp = requires(E e) {
    E::Load;
    E::Clear;
    E::DontCare; 
};

template<typename E>
concept IsAttachmentStoreOp = requires(E e) {
    E::Store;
    E::DontCare;
    E::None;
};

template<typename E>
concept IsRenderingConfigOption = requires(E e) {
    E::Resume;
    E::Suspend;
};

static_assert(IsAttachmentLoadOp<AttachmentLoadOp>);
static_assert(IsAttachmentStoreOp<AttachmentStoreOp>);
static_assert(IsCommandBufferUsage<CommandBufferUsage>);
static_assert(IsCommandPoolConfigOption<CommandPoolConfigOption>);
static_assert(IsCommandResources<CommandResources>);
static_assert(IsMemoryAccess<MemoryAccess>);
static_assert(IsPipelineStage<PipelineStage>);
static_assert(IsRenderingConfigOption<RenderingConfigOption>);
static_assert(IsResolveMode<ResolveMode>);
}

using CommandPoolConfigFlags    = Flags<CommandPoolConfigOption>;
using CommandBufferUsageFlags   = Flags<CommandBufferUsage>;
using PipelineStageFlags        = Flags<PipelineStage>;
using MemoryAccessFlags         = Flags<MemoryAccess>;
using RenderingConfigFlags      = Flags<RenderingConfigOption>;

struct CommandPoolConfig {
    CommandPoolConfigFlags  flags;
    QueueFamilyID           queue_family;
};

struct CommandBufferBeginConfig {
    CommandBufferUsageFlags usage;
};

struct MemoryBarrier {
    PipelineStageFlags  src_stages;
    MemoryAccessFlags   src_accesses;
    PipelineStageFlags  dst_stages;
    MemoryAccessFlags   dst_accesses;
};

struct QueueFamilyTransfer {
    QueueFamily::ID src;
    QueueFamily::ID dst;
};

struct BufferBarrier {
    MemoryBarrier       memory_barrier;
    QueueFamilyTransfer queue_family_transfer;
    Buffer              buffer;
    size_t              offset;
    size_t              size;
};

struct ImageBarrier {
    MemoryBarrier               memory_barrier;
    ImageLayout                 old_layout;
    ImageLayout                 new_layout;
    QueueFamilyTransfer         queue_family_transfer;
    Image                       image;
    ImageSubresourceRange       subresource_range;
};

struct DependencyConfig {
    std::span<const MemoryBarrier>  memory_barriers;
    std::span<const BufferBarrier>  buffer_barriers;
    std::span<const ImageBarrier>   image_barriers;
    bool                            by_region: 1;
};

struct ClearValue {
    union {
        struct {
            float r, g, b, a;
        } color;
        struct {
            float depth;
            unsigned stencil;
        };
    };
};

struct Viewport {
    float x, y;
    float width, height;
    float min_depth, max_depth;
};

struct Rect2D {
    int         x, y;
    unsigned    width, height;
};

struct RenderingAttachment {
    ImageView           view;
    ImageLayout         layout;
    struct {
        ResolveMode     mode;
        ImageView       view;
        ImageLayout     layout;
    }                   resolve;
    AttachmentLoadOp    load_op;
    AttachmentStoreOp   store_op;
    ClearValue          clear_value;
};

struct RenderingConfig {
    RenderingConfigFlags                    flags;
    Rect2D                                  render_area;
    std::span<const RenderingAttachment>    color_attachments;
    RenderingAttachment                     depth_attachment;
    RenderingAttachment                     stencil_attachment; 
};

struct DrawConfig {
    unsigned first_vertex;
    unsigned vertex_count;
    unsigned first_instance;
    unsigned instance_count;
};

CommandPool CreateCommandPool(Context ctx, const CommandPoolConfig& config);
void DestroyCommandPool(Context ctx, CommandPool pool);
void ResetCommandPool(Context ctx, CommandPool pool, CommandResources resources);
void TrimCommandPool(Context ctx, CommandPool pool);

void AllocateCommandBuffers(
    Context ctx, CommandPool pool,
    std::span<CommandBuffer> cmd_buffers
);
void FreeCommandBuffers(
    Context ctx, CommandPool pool,
    std::span<CommandBuffer> cmd_buffers
);
void ResetCommandBuffer(CommandBuffer cmd_buffer, CommandResources resources);

void BeginCommandBuffer(CommandBuffer cmd_buffer, const CommandBufferBeginConfig& begin_config);
void EndCommandBuffer(CommandBuffer cmd_buffer);

void CmdPipelineBarrier(
    CommandBuffer cmd_buffer, const DependencyConfig& config
);

void CmdBeginRendering(CommandBuffer cmd_buffer, const RenderingConfig& config);
void CmdEndRendering(CommandBuffer cmd_buffer);

void CmdSetViewports(CommandBuffer cmd_buffer, std::span<const Viewport> viewports);
void CmdSetScissors(CommandBuffer cmd_buffer, std::span<const Rect2D> scissors);

void CmdBindGraphicsPipeline(CommandBuffer cmd_buffer, Pipeline pipeline);

void CmdDraw(CommandBuffer cmd_buffer, const DrawConfig& config);
}
