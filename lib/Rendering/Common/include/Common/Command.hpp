#pragma once
#include "Device.hpp"
#include "Image.hpp"

#include <span>

namespace R1 {
struct CommandPoolCapabilities {
    bool transient: 1;
    bool reset_command_buffer: 1;
};

struct CommandPoolConfig {
    CommandPoolCapabilities capabilities;
    QueueFamily::ID queue_family;
};

enum class CommandResources {
    Release,
    Keep,
};

struct CommandBufferUsage {
    bool one_time_submit: 1;
    bool simultaneous_use: 1;
};

struct CommandBufferBeginConfig {
    CommandBufferUsage usage;
};

struct PipelineStages {
    bool draw_indirect: 1;
    bool index_input: 1;
    bool vertex_attribute_input: 1;
    bool vertex_input: 1;
    bool vertex_shader: 1;
    bool tessellation_control_shader: 1;
    bool tessellation_evaluation_shader: 1;
    bool geometry_shader: 1;
    bool pre_rasterization_shaders: 1;
    bool fragment_shader: 1;
    bool early_fragment_tests: 1;
    bool late_fragment_tests: 1;
    bool color_attachment_output: 1;
    bool all_graphics: 1;
    bool copy: 1;
    bool resolve: 1;
    bool blit: 1;
    bool clear: 1;
    bool all_transfer: 1;
    bool compute_shader: 1;
    bool all_commands: 1;
};

struct MemoryAccesses {
    bool indirect_command_read: 1;
    bool index_read: 1;
    bool vertex_attribute_read: 1;
    bool uniform_read: 1;
    bool shader_sampled_read: 1;
    bool shader_storage_read: 1;
    bool shader_read: 1;
    bool shader_storage_write: 1;
    bool shader_write: 1;
    bool input_attachment_read: 1;
    bool color_attachment_read: 1;
    bool color_attachment_write: 1;
    bool depth_stencil_attachment_read: 1;
    bool depth_stencil_attachment_write: 1;
    bool transfer_read: 1;
    bool transfer_write: 1;
    bool memory_read: 1;
    bool memory_write: 1;
};

struct MemoryBarrier {
    PipelineStages src_stages;
    MemoryAccesses src_accesses;
    PipelineStages dst_stages;
    MemoryAccesses dst_accesses;
};

struct QueueFamilyTransfer {
    QueueFamily::ID src;
    QueueFamily::ID dst;
};

template<class Buffer>
struct BufferBarrierBase {
    MemoryBarrier       memory_barrier;
    QueueFamilyTransfer queue_family_transfer;
    Buffer              buffer;
    size_t              offset;
    size_t              size;
};

template<class Image>
struct ImageBarrierBase {
    MemoryBarrier           memory_barrier;
    ImageLayout             old_layout;
    ImageLayout             new_layout;
    QueueFamilyTransfer     queue_family_transfer;
    Image                   image;
    ImageSubresourceRange   subresource_range;
};

template<class BufferBarrier, class ImageBarrier>
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

struct RenderingContinuation {
    bool resume: 1;
    bool suspend: 1;
};

template<class RenderingAttachment, class Rect2D>
struct RenderingConfigBase {
    RenderingContinuation                   continuation;
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
