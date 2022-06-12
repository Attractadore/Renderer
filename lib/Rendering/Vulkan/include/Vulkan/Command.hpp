#pragma once
#include "Buffer.hpp"
#include "Common/Command.hpp"
#include "Image.hpp"
#include "Pipeline.hpp"

namespace R1 {
enum class CommandPoolConfigOption {
    Transient               = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT,
    AllowCommandBufferReset = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
};

enum class CommandResources {
    Keep = 0,
    Release = VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT,
};

enum class CommandBufferUsage {
    OneTimeSubmit   = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
    Simultaneous    = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT,
};

enum class PipelineStage: VkPipelineStageFlags2 {
    DrawIndirect                = VK_PIPELINE_STAGE_2_DRAW_INDIRECT_BIT,

    IndexInput                  = VK_PIPELINE_STAGE_2_INDEX_INPUT_BIT,
    VertexAttributeInput        = VK_PIPELINE_STAGE_2_VERTEX_ATTRIBUTE_INPUT_BIT,
    VertexInput                 = VK_PIPELINE_STAGE_2_VERTEX_INPUT_BIT,

    VertexShader                = VK_PIPELINE_STAGE_2_VERTEX_SHADER_BIT,
    TesselationControlShader    = VK_PIPELINE_STAGE_2_TESSELLATION_CONTROL_SHADER_BIT,
    TesselationEvaluationShader = VK_PIPELINE_STAGE_2_TESSELLATION_EVALUATION_SHADER_BIT,
    GeometryShader              = VK_PIPELINE_STAGE_2_GEOMETRY_SHADER_BIT,
    PreRasterizationShaders     = VK_PIPELINE_STAGE_2_PRE_RASTERIZATION_SHADERS_BIT,

    FragmentShader              = VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT,
    EarlyFragmentTests          = VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT,
    LateFragmentTests           = VK_PIPELINE_STAGE_2_LATE_FRAGMENT_TESTS_BIT,
    ColorAttachmentOutput       = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,

    AllGraphics                 = VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT,

    ComputeShader               = VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT,
    AllCompute                  = ComputeShader,

    Copy                        = VK_PIPELINE_STAGE_2_COPY_BIT,
    Resolve                     = VK_PIPELINE_STAGE_2_RESOLVE_BIT,
    Blit                        = VK_PIPELINE_STAGE_2_BLIT_BIT,
    Clear                       = VK_PIPELINE_STAGE_2_CLEAR_BIT,
    AllTransfer                 = VK_PIPELINE_STAGE_2_ALL_TRANSFER_BIT,

    AllCommands                 = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT,
};
using PipelineStageFlags = Flags<PipelineStage>;

enum class MemoryAccess: VkAccessFlags2 {
    IndirectCommandRead         = VK_ACCESS_2_INDIRECT_COMMAND_READ_BIT,

    IndexRead                   = VK_ACCESS_2_INDEX_READ_BIT,
    VertexAttributeRead         = VK_ACCESS_2_VERTEX_ATTRIBUTE_READ_BIT,
    VertexRead                  = IndexRead | VertexAttributeRead,

    ShaderUniformRead           = VK_ACCESS_2_UNIFORM_READ_BIT,
    ShaderSampledRead           = VK_ACCESS_2_SHADER_SAMPLED_READ_BIT,
    ShaderStorageRead           = VK_ACCESS_2_SHADER_STORAGE_READ_BIT,
    ShaderRead                  = VK_ACCESS_2_SHADER_READ_BIT,

    ShaderStorageWrite          = VK_ACCESS_2_SHADER_STORAGE_WRITE_BIT,
    ShaderWrite                 = VK_ACCESS_2_SHADER_WRITE_BIT,

    ColorAttachmentRead         = VK_ACCESS_2_COLOR_ATTACHMENT_READ_BIT,
    DepthAttachmentRead         = VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_READ_BIT,
    StencilAttachmentRead       = VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_READ_BIT,
    DepthStencilAttachmentRead  = DepthAttachmentRead | StencilAttachmentRead,
    AttachmentRead              = ColorAttachmentRead | DepthStencilAttachmentRead,

    ColorAttachmentWrite        = VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT,
    DepthAttachmentWrite        = VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
    StencilAttachmentWrite      = VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
    DepthStencilAttachmentWrite = DepthAttachmentWrite | StencilAttachmentWrite,
    AttachmentWrite             = ColorAttachmentWrite | DepthStencilAttachmentWrite,

    TransferRead                = VK_ACCESS_2_TRANSFER_READ_BIT,
    TransferWrite               = VK_ACCESS_2_TRANSFER_WRITE_BIT,

    MemoryRead                  = VK_ACCESS_2_MEMORY_READ_BIT,
    MemoryWrite                 = VK_ACCESS_2_MEMORY_WRITE_BIT,
};
using MemoryAccessFlags = Flags<MemoryAccess>;

enum class ResolveMode {
    None        = VK_RESOLVE_MODE_NONE,
    SampleZero  = VK_RESOLVE_MODE_SAMPLE_ZERO_BIT,
    Average     = VK_RESOLVE_MODE_AVERAGE_BIT,
    Min         = VK_RESOLVE_MODE_MIN_BIT,
    Max         = VK_RESOLVE_MODE_MAX_BIT,
};

enum class AttachmentLoadOp {
    Load        = VK_ATTACHMENT_LOAD_OP_LOAD,
    Clear       = VK_ATTACHMENT_LOAD_OP_CLEAR,
    DontCare    = VK_ATTACHMENT_LOAD_OP_DONT_CARE, 
};

enum class AttachmentStoreOp {
    Store       = VK_ATTACHMENT_STORE_OP_STORE,
    DontCare    = VK_ATTACHMENT_STORE_OP_DONT_CARE,
    None        = VK_ATTACHMENT_STORE_OP_NONE,
};

enum class RenderingConfigOption {
    Resume = VK_RENDERING_RESUMING_BIT_KHR,
    Suspend = VK_RENDERING_SUSPENDING_BIT,
};
};

namespace R1::VK {
using CommandPool = VkCommandPool;
using CommandBuffer = VkCommandBuffer;

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

using MemoryBarrier = MemoryBarrierBase<PipelineStageFlags, MemoryAccessFlags>;
using BufferBarrier = BufferBarrierBase<MemoryBarrier, Buffer>;
using ImageBarrier  = ImageBarrierBase<MemoryBarrier, Image>;
using DependencyConfig = DependencyConfigBase<
    MemoryBarrier, BufferBarrier, ImageBarrier>;
void CmdPipelineBarrier(
    CommandBuffer cmd_buffer, const DependencyConfig& config
);

struct Viewport {
    union {
        VkViewport vk_viewport;
        struct {
            float x, y;
            float width, height;
            float min_depth, max_depth;
        };
    };
};
static_assert(offsetof(Viewport, x) == offsetof(Viewport, vk_viewport.x));
static_assert(offsetof(Viewport, y) == offsetof(Viewport, vk_viewport.y));
static_assert(offsetof(Viewport, width) == offsetof(Viewport, vk_viewport.width));
static_assert(offsetof(Viewport, height) == offsetof(Viewport, vk_viewport.height));
static_assert(offsetof(Viewport, min_depth) == offsetof(Viewport, vk_viewport.minDepth));
static_assert(offsetof(Viewport, max_depth) == offsetof(Viewport, vk_viewport.maxDepth));
static_assert(sizeof(Viewport) == sizeof(VkViewport));

struct Rect2D {
    union {
        VkRect2D vk_rect;
        struct {
            int x, y;
            unsigned width, height;
        };
    };
};
static_assert(offsetof(Rect2D, x) == offsetof(Rect2D, vk_rect.offset.x));
static_assert(offsetof(Rect2D, y) == offsetof(Rect2D, vk_rect.offset.y));
static_assert(offsetof(Rect2D, width) == offsetof(Rect2D, vk_rect.extent.width));
static_assert(offsetof(Rect2D, height) == offsetof(Rect2D, vk_rect.extent.height));
static_assert(sizeof(Rect2D) == sizeof(VkRect2D));

using RenderingAttachment = RenderingAttachmentBase<ImageView>;
using RenderingConfig = RenderingConfigBase<RenderingAttachment, Rect2D>;
void CmdBeginRendering(CommandBuffer cmd_buffer, const RenderingConfig& config);
void CmdEndRendering(CommandBuffer cmd_buffer);

void CmdSetViewports(CommandBuffer cmd_buffer, std::span<const Viewport> viewports);
void CmdSetScissors(CommandBuffer cmd_buffer, std::span<const Rect2D> scissors);

void CmdBindGraphicsPipeline(CommandBuffer cmd_buffer, Pipeline pipeline);

void CmdDraw(CommandBuffer cmd_buffer, const DrawConfig& config);
}
