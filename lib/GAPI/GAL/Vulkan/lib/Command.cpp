#include "CommandImpl.hpp"
#include "Common/Vector.hpp"
#include "ContextImpl.hpp"

namespace R1::GAL {
CommandPool CreateCommandPool(
    Context ctx, const CommandPoolConfig& config
) {
    VkCommandPoolCreateInfo create_info = {
        .sType = sType(create_info),
        .flags = static_cast<VkCommandPoolCreateFlags>(
            config.flags.Extract()),
        .queueFamilyIndex = config.queue_family,
    };

    VkCommandPool pool = VK_NULL_HANDLE;
    vkCreateCommandPool(ctx->device.get(), &create_info, nullptr, &pool);
    if (!pool) {
        throw std::runtime_error{
            "Vulkan: Failed to create command pool"};
    }

    return pool;
};

void DestroyCommandPool(Context ctx, CommandPool pool) {
    vkDestroyCommandPool(ctx->device.get(), pool, nullptr);
}

void ResetCommandPool(
    Context ctx, CommandPool pool, CommandResources resources
) {
    auto r = vkResetCommandPool(
        ctx->device.get(), pool,
        static_cast<VkCommandPoolResetFlags>(resources));
    if (r) {
        throw std::runtime_error{
            "Vulkan: Failed to reset command pool"};
    }
};

void TrimCommandPool(Context ctx, CommandPool pool) {
    vkTrimCommandPool(ctx->device.get(), pool, 0);
}

void AllocateCommandBuffers(
    Context ctx, CommandPool pool,
    std::span<CommandBuffer> cmd_buffers
) {
    VkCommandBufferAllocateInfo alloc_info = {
        .sType = sType(alloc_info),
        .commandPool = pool,
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = static_cast<uint32_t>(cmd_buffers.size()),
    };

    auto r = vkAllocateCommandBuffers(
        ctx->device.get(), &alloc_info, cmd_buffers.data());
    if (r) {
        throw std::runtime_error{
            "Vulkan: Failed to allocate command buffers"};
    }
}

void FreeCommandBuffers(
    Context ctx, CommandPool pool,
    std::span<CommandBuffer> cmd_buffers
) {
    vkFreeCommandBuffers(
        ctx->device.get(), pool, cmd_buffers.size(), cmd_buffers.data());
}

void ResetCommandBuffer(
    CommandBuffer cmd_buffer, CommandResources resources
) {
    auto r = vkResetCommandBuffer(
        cmd_buffer,
        static_cast<VkCommandBufferResetFlags>(resources));
    if (r) {
        throw std::runtime_error{
            "Vulkan: Failed to reset command buffer"};
    }
}

void BeginCommandBuffer(
    CommandBuffer cmd_buffer,
    const CommandBufferBeginConfig& begin_config
) {
    VkCommandBufferBeginInfo begin_info = {
        .sType = sType(begin_info),
        .flags = static_cast<VkCommandBufferUsageFlags>(begin_config.usage.Extract()),
    };
    auto r = vkBeginCommandBuffer(cmd_buffer, &begin_info);
    if (r) {
        throw std::runtime_error{
            "Vulkan: Failed to begin recording command buffer"};
    }
}

void EndCommandBuffer(
    CommandBuffer cmd_buffer
) {
    auto r = vkEndCommandBuffer(cmd_buffer);
    if (r) {
        throw std::runtime_error{
            "Vulkan: Failed to record command buffer"};
    }
}

void CmdPipelineBarrier(CommandBuffer cmd_buffer, const DependencyConfig& config) {
    auto memory_barriers = vec_from_range(
        std::views::transform(
            config.memory_barriers, MemoryBarrierToVK));
    auto buffer_barriers = vec_from_range(
        std::views::transform(
            config.buffer_barriers, BufferBarrierToVK));
    auto image_barriers = vec_from_range(
        std::views::transform(
            config.image_barriers, ImageBarrierToVK));
    VkDependencyInfo dep_info = {
        .sType = sType(dep_info),
        .dependencyFlags = config.by_region ? VK_DEPENDENCY_BY_REGION_BIT: 0u,
        .memoryBarrierCount =
            static_cast<uint32_t>(memory_barriers.size()),
        .pMemoryBarriers = memory_barriers.data(),
        .bufferMemoryBarrierCount =
            static_cast<uint32_t>(buffer_barriers.size()),
        .pBufferMemoryBarriers = buffer_barriers.data(),
        .imageMemoryBarrierCount =
            static_cast<uint32_t>(image_barriers.size()),
        .pImageMemoryBarriers = image_barriers.data(),
    };
    vkCmdPipelineBarrier2(cmd_buffer, &dep_info);
}

void CmdBeginRendering(
    CommandBuffer cmd_buffer, const RenderingConfig& config
) {
    auto color_attachments = vec_from_range(
        std::views::transform(
            config.color_attachments, RenderingAttachmentToVK));
    auto depth_attachment =
        RenderingAttachmentToVK(config.depth_attachment);
    auto stencil_attachment =
        RenderingAttachmentToVK(config.stencil_attachment);
    VkRenderingInfo rendering_info = {
        .sType = sType(rendering_info),
        .flags = static_cast<VkRenderingFlags>(config.flags.Extract()),
        .renderArea = Rect2DToVK(config.render_area),
        .layerCount = 1,
        .colorAttachmentCount =
            static_cast<uint32_t>(color_attachments.size()),
        .pColorAttachments = color_attachments.data(),
        .pDepthAttachment = &depth_attachment,
        .pStencilAttachment = &stencil_attachment,
    };
    vkCmdBeginRendering(cmd_buffer, &rendering_info);
}

void CmdEndRendering(CommandBuffer cmd_buffer) {
    vkCmdEndRendering(cmd_buffer);
}

void CmdSetViewports(CommandBuffer cmd_buffer, std::span<const Viewport> viewports) {
    vkCmdSetViewportWithCount(
        cmd_buffer, viewports.size(),
        reinterpret_cast<const VkViewport*>(viewports.data()));
}

void CmdSetScissors(CommandBuffer cmd_buffer, std::span<const Rect2D> scissors) {
    vkCmdSetScissorWithCount(
        cmd_buffer, scissors.size(),
        reinterpret_cast<const VkRect2D*>(scissors.data()));
}

void CmdBindGraphicsPipeline(CommandBuffer cmd_buffer, Pipeline pipeline) {
    vkCmdBindPipeline(cmd_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
}

void CmdDraw(CommandBuffer cmd_buffer, const DrawConfig& config) {
    vkCmdDraw(cmd_buffer, config.vertex_count, config.instance_count, config.first_vertex, config.first_instance);
}
}
