#pragma once
#include "Types.hpp"

namespace R1::GAL {
CommandPool CreateCommandPool(Context ctx, const CommandPoolConfig& config);
void DestroyCommandPool(Context ctx, CommandPool pool);
void ResetCommandPool(Context ctx, CommandPool pool, CommandResources resources);
void TrimCommandPool(Context ctx, CommandPool pool);

void AllocateCommandBuffers(
    Context ctx, CommandPool pool,
    std::span<const CommandBuffer> cmd_buffers
);
void FreeCommandBuffers(
    Context ctx, CommandPool pool,
    std::span<const CommandBuffer> cmd_buffers
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
