#include "CommandImpl.hpp"
#include "Common/Vector.hpp"
#include "ContextImpl.hpp"

namespace R1::GAL {
CommandPool CreateCommandPool(
    Context ctx, const CommandPoolConfig& config
) {
    VkCommandPoolCreateInfo create_info = {
        .sType = SType(create_info),
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
        .sType = SType(alloc_info),
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
        .sType = SType(begin_info),
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
    static thread_local std::vector<VkMemoryBarrier2> memory_barriers;
    static thread_local std::vector<VkBufferMemoryBarrier2> buffer_barriers;
    static thread_local std::vector<VkImageMemoryBarrier2> image_barriers;
    auto mv = std::views::transform(
            config.memory_barriers, MemoryBarrierToVK);
    auto bv = std::views::transform(
            config.buffer_barriers, BufferBarrierToVK);
    auto iv = std::views::transform(
            config.image_barriers, ImageBarrierToVK);
    memory_barriers.assign(mv.begin(), mv.end());
    buffer_barriers.assign(bv.begin(), bv.end());
    image_barriers.assign(iv.begin(), iv.end());
    VkDependencyInfo dep_info = {
        .sType = SType(dep_info),
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
    static thread_local std::vector<VkRenderingAttachmentInfo>
        color_attachments;
    auto cv = std::views::transform(
        config.color_attachments, RenderingAttachmentToVK);
    color_attachments.assign(cv.begin(), cv.end());
    auto depth_attachment =
        RenderingAttachmentToVK(config.depth_attachment);
    auto stencil_attachment =
        RenderingAttachmentToVK(config.stencil_attachment);
    VkRenderingInfo rendering_info = {
        .sType = SType(rendering_info),
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

void CmdSetViewports(
    CommandBuffer cmd_buffer, std::span<const Viewport> viewports
) {
    static thread_local std::vector<VkViewport> vk_viewports;
    auto v = std::views::transform(viewports, ViewportToVK);
    vk_viewports.assign(v.begin(), v.end());
    vkCmdSetViewportWithCount(
        cmd_buffer, vk_viewports.size(), vk_viewports.data());
}

void CmdSetScissors(
    CommandBuffer cmd_buffer, std::span<const Rect2D> scissors
) {
    static thread_local std::vector<VkRect2D> vk_rects;
    auto v = std::views::transform(scissors, Rect2DToVK);
    vk_rects.assign(v.begin(), v.end());
    vkCmdSetScissorWithCount(
        cmd_buffer, vk_rects.size(), vk_rects.data());
}

void CmdBindGraphicsPipeline(CommandBuffer cmd_buffer, Pipeline pipeline) {
    vkCmdBindPipeline(cmd_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
}

void CmdDraw(CommandBuffer cmd_buffer, const DrawConfig& config) {
    vkCmdDraw(cmd_buffer, config.vertex_count, config.instance_count, config.first_vertex, config.first_instance);
}
}
