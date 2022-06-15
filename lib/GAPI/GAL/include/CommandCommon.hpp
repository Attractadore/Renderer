#pragma once
#include "DeviceCommon.hpp"
#include "ImageCommon.hpp"

#include <span>

namespace R1::GAL {
namespace Detail {
template<typename Traits>
struct CommandPoolConfigBase {
    Traits::CommandPoolConfigFlags  flags;
    QueueFamilyID                   queue_family;
};

template<typename Traits>
struct CommandBufferBeginConfigBase {
    Traits::CommandBufferUsageFlags usage;
};

template<typename Traits>
struct MemoryBarrierBase {
    Traits::PipelineStageFlags  src_stages;
    Traits::MemoryAccessFlags   src_accesses;
    Traits::PipelineStageFlags  dst_stages;
    Traits::MemoryAccessFlags   dst_accesses;
};
}

struct QueueFamilyTransfer {
    QueueFamilyID src;
    QueueFamilyID dst;
};

namespace Detail {
template<typename Traits>
struct BufferBarrierBase {
    MemoryBarrierBase<Traits>   memory_barrier;
    QueueFamilyTransfer         queue_family_transfer;
    Traits::Buffer              buffer;
    size_t                      offset;
    size_t                      size;
};

template<typename Traits>
struct ImageBarrierBase {
    MemoryBarrierBase<Traits>           memory_barrier;
    Traits::ImageLayout                 old_layout;
    Traits::ImageLayout                 new_layout;
    QueueFamilyTransfer                 queue_family_transfer;
    Traits::Image                       image;
    ImageSubresourceRangeBase<Traits>   subresource_range;
};

template<typename Traits>
struct DependencyConfigBase {
    std::span<const MemoryBarrierBase<Traits>> memory_barriers;
    std::span<const BufferBarrierBase<Traits>> buffer_barriers;
    std::span<const ImageBarrierBase<Traits>>  image_barriers;
    bool                                                by_region: 1;
};
}

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

namespace Detail {
template<typename Traits>
struct RenderingAttachmentBase {
    Traits::ImageView           view;
    Traits::ImageLayout         layout;
    struct {
        Traits::ResolveMode     mode;
        Traits::ImageView       view;
        Traits::ImageLayout     layout;
    }                           resolve;
    Traits::AttachmentLoadOp    load_op;
    Traits::AttachmentStoreOp   store_op;
    ClearValue                  clear_value;
};

template<typename Traits>
struct RenderingConfigBase {
    Traits::RenderingConfigFlags                        flags;
    Traits::Rect2D                                      render_area;
    std::span<const RenderingAttachmentBase<Traits>>    color_attachments;
    RenderingAttachmentBase<Traits>                     depth_attachment;
    RenderingAttachmentBase<Traits>                     stencil_attachment; 
};
}

struct DrawConfig {
    unsigned first_vertex;
    unsigned vertex_count;
    unsigned first_instance;
    unsigned instance_count;
};
}
