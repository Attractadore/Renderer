#pragma once
#include "Device.hpp"
#include "Image.hpp"

#include <span>

namespace R1 {
enum class CommandPoolConfigOption;
using CommandPoolFlags = Flags<CommandPoolConfigOption>;

struct CommandPoolConfig {
    CommandPoolFlags    flags;
    QueueFamily::ID     queue_family;
};

enum class CommandResources;

enum class CommandBufferUsage;
using CommandBufferUsageFlags = Flags<CommandBufferUsage>;

struct CommandBufferBeginConfig {
    CommandBufferUsageFlags usage;
};

template<typename PipelineStageFlags, typename MemoryAccessFlags>
struct MemoryBarrierBase {
    PipelineStageFlags  src_stages;
    MemoryAccessFlags   src_accesses;
    PipelineStageFlags  dst_stages;
    MemoryAccessFlags   dst_accesses;
};

struct QueueFamilyTransfer {
    QueueFamily::ID src;
    QueueFamily::ID dst;
};

template<class MemoryBarrier, class Buffer>
struct BufferBarrierBase {
    MemoryBarrier       memory_barrier;
    QueueFamilyTransfer queue_family_transfer;
    Buffer              buffer;
    size_t              offset;
    size_t              size;
};

template<class MemoryBarrier, class Image>
struct ImageBarrierBase {
    MemoryBarrier           memory_barrier;
    ImageLayout             old_layout;
    ImageLayout             new_layout;
    QueueFamilyTransfer     queue_family_transfer;
    Image                   image;
    ImageSubresourceRange   subresource_range;
};

template<class MemoryBarrier, class BufferBarrier, class ImageBarrier>
struct DependencyConfigBase {
    std::span<const MemoryBarrier>  memory_barriers;
    std::span<const BufferBarrier>  buffer_barriers;
    std::span<const ImageBarrier>   image_barriers;
    bool                            by_region: 1;
};

enum class ResolveMode;
enum class AttachmentLoadOp;
enum class AttachmentStoreOp;

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

template<class ImageView>
struct RenderingAttachmentBase {
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

enum class RenderingConfigOption;
using RenderingConfigFlags = Flags<RenderingConfigOption>;

template<class RenderingAttachment, class Rect2D>
struct RenderingConfigBase {
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
}
