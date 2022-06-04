#pragma once
#include "Common/Command.hpp"
#include "Context.hpp"

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
}
