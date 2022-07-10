#pragma once
#if GAL_USE_VULKAN
#include "VulkanCommand.hpp"
#endif

#include "Buffer.hpp"
#include "Pipeline.hpp"
#include "PipelineStages.hpp"
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
static_assert(IsRenderingConfigOption<RenderingConfigOption>);
static_assert(IsResolveMode<ResolveMode>);
}

using CommandPoolConfigFlags    = Flags<CommandPoolConfigOption>;
using CommandBufferUsageFlags   = Flags<CommandBufferUsage>;
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

struct Offset3D {
    int x, y, z;
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

struct ImageSubresourceLayers {
    GAL::ImageAspectFlags   aspects;
    unsigned                mip_level;
    unsigned                first_array_layer;
    unsigned                array_layer_count;
};

struct BlitRegion {
    ImageSubresourceLayers    src_subresource;
    std::array<Offset3D, 2>   src_offsets;
    ImageSubresourceLayers    dst_subresource;
    std::array<Offset3D, 2>   dst_offsets;
};

struct ImageBlitConfig {
    Image                       src_image;
    ImageLayout                 src_layout;
    Image                       dst_image;
    ImageLayout                 dst_layout;
    std::span<const BlitRegion> regions;
    Filter                      filter; 
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
void ResetCommandBuffer(
    Context ctx, CommandPool pool,
    CommandBuffer cmd_buffer, CommandResources resources
);

void BeginCommandBuffer(
    Context ctx,
    CommandBuffer cmd_buffer, const CommandBufferBeginConfig& begin_config
);
void EndCommandBuffer(Context ctx, CommandBuffer cmd_buffer);

void CmdPipelineBarrier(
    Context ctx, CommandBuffer cmd_buffer, const DependencyConfig& config
);

void CmdBeginRendering(
    Context ctx, CommandBuffer cmd_buffer, const RenderingConfig& config
);
void CmdEndRendering(Context ctx, CommandBuffer cmd_buffer);

void CmdSetViewports(
    Context ctx,
    CommandBuffer cmd_buffer, std::span<const Viewport> viewports
);
void CmdSetScissors(
    Context ctx,
    CommandBuffer cmd_buffer, std::span<const Rect2D> scissors
);

void CmdBindGraphicsPipeline(
    Context ctx, CommandBuffer cmd_buffer, Pipeline pipeline
);

void CmdDraw(
    Context ctx, CommandBuffer cmd_buffer, const DrawConfig& config
);

void CmdBlitImage(
    Context ctx, CommandBuffer cmd_buffer, const ImageBlitConfig& config
);

struct BufferCopyRegion {
    size_t src_offset;
    size_t dst_offset;
    size_t size;
};

struct BufferCopyConfig {
    Buffer                              src;
    Buffer                              dst;
    std::span<const BufferCopyRegion>   regions;
};

void CmdCopyBuffer(
    Context ctx, CommandBuffer cmd_buffer, const BufferCopyConfig& config
);

struct VertexBufferBindConfig {
    unsigned                        first_binding;
    std::span<const GAL::Buffer>    buffers;
    std::span<const size_t>         offsets;
    std::span<const size_t>         sizes;
    std::span<const size_t>         strides;
};

void CmdBindVertexBuffers(
    Context ctx, CommandBuffer cmd_buffer, const VertexBufferBindConfig& config
);

struct DescriptorSetBindConfig {
    PipelineLayout                  layout;
    unsigned                        first_set;
    std::span<const DescriptorSet>  sets;
    std::span<const unsigned>       dynamic_offsets;
};

void CmdBindGraphicsPipelineDescriptorSets(
    Context ctx, CommandBuffer cmd_buffer,
    const DescriptorSetBindConfig& config
);
}
