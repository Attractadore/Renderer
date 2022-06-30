#include "CommandImpl.inl"
#include "Common/Vector.hpp"
#include "ContextImpl.hpp"

#include <cstring>

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
    VkCommandPool pool;
    ThrowIfFailed(
        ctx->CreateCommandPool(&create_info, &pool),
        "Vulkan: Failed to create command pool");
    return pool;
};

void DestroyCommandPool(Context ctx, CommandPool pool) {
    ctx->DestroyCommandPool(pool);
}

void ResetCommandPool(
    Context ctx, CommandPool pool, CommandResources resources
) {
    ThrowIfFailed(
        ctx->ResetCommandPool(
            pool, static_cast<VkCommandPoolResetFlags>(resources)),
        "Vulkan: Failed to reset command pool");
};

void TrimCommandPool(Context ctx, CommandPool pool) {
    ctx->TrimCommandPool(pool, 0);
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
    ThrowIfFailed(
        ctx->AllocateCommandBuffers(&alloc_info, cmd_buffers.data()),
        "Vulkan: Failed to allocate command buffers");
}

void FreeCommandBuffers(
    Context ctx, CommandPool pool,
    std::span<CommandBuffer> cmd_buffers
) {
    auto cnt = cmd_buffers.size();
    if (cnt) {
        ctx->FreeCommandBuffers(pool, cnt, cmd_buffers.data());
    }
}

void ResetCommandBuffer(
    Context ctx, CommandPool,
    CommandBuffer cmd_buffer, CommandResources resources
) {
    ThrowIfFailed(
        ctx->ResetCommandBuffer(
            cmd_buffer,
            static_cast<VkCommandBufferResetFlags>(resources)),
        "Vulkan: Failed to reset command buffer");
}

void BeginCommandBuffer(
    Context ctx,
    CommandBuffer cmd_buffer,
    const CommandBufferBeginConfig& begin_config
) {
    VkCommandBufferBeginInfo begin_info = {
        .sType = SType(begin_info),
        .flags = static_cast<VkCommandBufferUsageFlags>(begin_config.usage.Extract()),
    };
    ThrowIfFailed(
        ctx->BeginCommandBuffer(cmd_buffer, &begin_info),
        "Vulkan: Failed to begin recording command buffer");
}

void EndCommandBuffer(Context ctx, CommandBuffer cmd_buffer) {
    ThrowIfFailed(
        ctx->EndCommandBuffer(cmd_buffer),
        "Vulkan: Failed to record command buffer");
}

void CmdPipelineBarrier(
    Context ctx,
    CommandBuffer cmd_buffer, const DependencyConfig& config
) {
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
        .dependencyFlags =
            config.by_region ? VK_DEPENDENCY_BY_REGION_BIT: 0u,
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
    ctx->CmdPipelineBarrier2(cmd_buffer, &dep_info);
}

void CmdBeginRendering(
    Context ctx,
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
    ctx->CmdBeginRendering(cmd_buffer, &rendering_info);
}

void CmdEndRendering(Context ctx, CommandBuffer cmd_buffer) {
    ctx->CmdEndRendering(cmd_buffer);
}

void CmdSetViewports(
    Context ctx,
    CommandBuffer cmd_buffer, std::span<const Viewport> viewports
) {
    static thread_local std::vector<VkViewport> vk_viewports;
    auto v = std::views::transform(viewports, ViewportToVK);
    vk_viewports.assign(v.begin(), v.end());
    ctx->CmdSetViewportWithCount(
        cmd_buffer, vk_viewports.size(), vk_viewports.data());
}

void CmdSetScissors(
    Context ctx,
    CommandBuffer cmd_buffer, std::span<const Rect2D> scissors
) {
    static thread_local std::vector<VkRect2D> vk_rects;
    auto v = std::views::transform(scissors, Rect2DToVK);
    vk_rects.assign(v.begin(), v.end());
    ctx->CmdSetScissorWithCount(
        cmd_buffer, vk_rects.size(), vk_rects.data());
}

void CmdBindGraphicsPipeline(
    Context ctx, CommandBuffer cmd_buffer, Pipeline pipeline
) {
    ctx->CmdBindPipeline(cmd_buffer,
        VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
}

void CmdDraw(
    Context ctx, CommandBuffer cmd_buffer, const DrawConfig& config
) {
    ctx->CmdDraw(cmd_buffer,
        config.vertex_count,
        config.instance_count,
        config.first_vertex,
        config.first_instance);
}

void CmdBlitImage(
    Context ctx, CommandBuffer cmd_buffer, const ImageBlitConfig& config
) {
    static thread_local std::vector<VkImageBlit2> regions;

    auto v = std::views::transform(config.regions,
        [] (const BlitRegion& reg) {
            VkImageBlit2 blit = {
                .sType = SType(blit),
                .srcSubresource = ImageSubresourceLayersToVK(reg.src_subresource),
                .dstSubresource = ImageSubresourceLayersToVK(reg.dst_subresource),
            };
            std::memcpy(blit.srcOffsets, &reg.src_offsets, sizeof(blit.srcOffsets));
            std::memcpy(blit.dstOffsets, &reg.dst_offsets, sizeof(blit.dstOffsets));
            return blit;
        });
    regions.assign(v.begin(), v.end());

    VkBlitImageInfo2 blit_info = {
        .sType = SType(blit_info),
        .srcImage = config.src_image->image,
        .srcImageLayout =
            static_cast<VkImageLayout>(config.src_layout),
        .dstImage = config.dst_image->image,
        .dstImageLayout = 
            static_cast<VkImageLayout>(config.dst_layout),
        .regionCount = regions.size(),
        .pRegions = regions.data(),
        .filter = static_cast<VkFilter>(config.filter),
    };
    ctx->CmdBlitImage2(cmd_buffer, &blit_info);
}
}
