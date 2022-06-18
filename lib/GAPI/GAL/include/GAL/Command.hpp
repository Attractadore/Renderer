#pragma once
#if GAL_USE_VULKAN
#include "VulkanCommand.hpp"
#endif

#include "Buffer.hpp"
#include "Pipeline.hpp"
#include "Image.hpp"

namespace R1::GAL {
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
