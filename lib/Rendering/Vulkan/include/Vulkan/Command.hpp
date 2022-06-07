#pragma once
#include "Buffer.hpp"
#include "Common/Command.hpp"
#include "Image.hpp"
#include "Pipeline.hpp"

namespace R1 {
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

using BufferBarrier = BufferBarrierBase<Buffer>;
using ImageBarrier = ImageBarrierBase<Image>;
using DependencyConfig =
    DependencyConfigBase<BufferBarrier, ImageBarrier>;
void CmdPipelineBarrier(CommandBuffer cmd_buffer, const DependencyConfig& config);

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
