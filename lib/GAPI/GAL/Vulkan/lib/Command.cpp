#include "BufferImpl.hpp"
#include "CommandImpl.inl"
#include "Common/Vector.hpp"
#include "ContextImpl.hpp"

#include <algorithm>
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
    Context ctx, CommandBuffer cmd_buffer,
    const DependencyConfig& config
) {
    DefaultSmallVector<VkMemoryBarrier2>
        memory_barriers(config.memory_barriers.size());
    DefaultSmallVector<VkBufferMemoryBarrier2>
        buffer_barriers(config.buffer_barriers.size());
    DefaultSmallVector<VkImageMemoryBarrier2>
        image_barriers(config.image_barriers.size());
    std::ranges::transform(config.memory_barriers,
        memory_barriers.begin(), MemoryBarrierToVK);
    std::ranges::transform(config.buffer_barriers,
        buffer_barriers.begin(), BufferBarrierToVK);
    std::ranges::transform(config.image_barriers,
        image_barriers.begin(), ImageBarrierToVK);
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
    Context ctx, CommandBuffer cmd_buffer,
    const RenderingConfig& config
) {
    DefaultSmallVector<VkRenderingAttachmentInfo>
        color_attachments(config.color_attachments.size());
    std::ranges::transform(config.color_attachments,
        color_attachments.begin(), RenderingAttachmentToVK);
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
    DefaultSmallVector<VkViewport> vk_viewports(viewports.size());
    std::ranges::transform(viewports, vk_viewports.begin(), ViewportToVK);
    ctx->CmdSetViewportWithCount(
        cmd_buffer, vk_viewports.size(), vk_viewports.data());
}

void CmdSetScissors(
    Context ctx,
    CommandBuffer cmd_buffer, std::span<const Rect2D> scissors
) {
    DefaultSmallVector<VkRect2D> vk_rects(scissors.size());
    std::ranges::transform(scissors, vk_rects.begin(), Rect2DToVK);
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

void CmdDrawIndexed(
    Context ctx, CommandBuffer cmd_buffer, const DrawIndexedConfig& config
) {
    ctx->CmdDrawIndexed(cmd_buffer,
        config.index_count,
        config.instance_count,
        config.first_index,
        config.vertex_offset,
        config.first_instance);
}

void CmdBlitImage(
    Context ctx, CommandBuffer cmd_buffer, const ImageBlitConfig& config
) {
    DefaultSmallVector<VkImageBlit2> regions(config.regions.size());
    std::ranges::transform(config.regions, regions.begin(),
        [] (const BlitRegion& reg) {
            VkImageBlit2 blit = {
                .sType = SType(blit),
                .srcSubresource = ImageSubresourceLayersToVK(reg.src_subresource),
                .dstSubresource = ImageSubresourceLayersToVK(reg.dst_subresource),
            };
            auto offset_transform = [] (const Offset3D& offset) {
                return VkOffset3D { offset.x, offset.y, offset.z };
            };
            std::ranges::transform(reg.src_offsets, blit.srcOffsets, offset_transform);
            std::ranges::transform(reg.dst_offsets, blit.dstOffsets, offset_transform);
            return blit;
        });
    VkBlitImageInfo2 blit_info = {
        .sType = SType(blit_info),
        .srcImage = config.src_image->image,
        .srcImageLayout =
            static_cast<VkImageLayout>(config.src_layout),
        .dstImage = config.dst_image->image,
        .dstImageLayout =
            static_cast<VkImageLayout>(config.dst_layout),
        .regionCount =
            static_cast<uint32_t>(regions.size()),
        .pRegions = regions.data(),
        .filter = static_cast<VkFilter>(config.filter),
    };
    ctx->CmdBlitImage2(cmd_buffer, &blit_info);
}

void CmdCopyBuffer(
    Context ctx, CommandBuffer cmd_buffer, const BufferCopyConfig& config
) {
    DefaultSmallVector<VkBufferCopy2> regions(config.regions.size());
    std::ranges::transform(config.regions, regions.begin(),
        [] (const BufferCopyRegion& region) {
            VkBufferCopy2 copy = {
                .sType = SType(copy),
                .srcOffset = region.src_offset,
                .dstOffset = region.dst_offset,
                .size = region.size,
            };
            return copy;
        });
    VkCopyBufferInfo2 copy_info = {
        .sType = SType(copy_info),
        .srcBuffer = config.src->buffer,
        .dstBuffer = config.dst->buffer,
        .regionCount =
            static_cast<uint32_t>(regions.size()),
        .pRegions = regions.data(),
    };
    ctx->CmdCopyBuffer2(cmd_buffer, &copy_info);
}
}

namespace R1 {
void GAL::CmdBindVertexBuffers(
    Context ctx, CommandBuffer cmd_buffer, const VertexBufferBindConfig& config
) {
    DefaultSmallVector<VkBuffer> buffers(config.buffers.size());
    std::ranges::transform(config.buffers, buffers.begin(),
        [] (GAL::Buffer buffer) {
            return buffer->buffer;
        });
    static_assert(sizeof(size_t) == sizeof(VkDeviceSize));
    ctx->CmdBindVertexBuffers2(cmd_buffer,
        config.first_binding, buffers.size(),
        buffers.data(),
        config.offsets.data(),
        config.sizes.data(),
        config.strides.data());
}

void GAL::CmdBindIndexBuffer(
    Context ctx, CommandBuffer cmd_buffer, const IndexBufferBindConfig& config
) {
    ctx->CmdBindIndexBuffer(cmd_buffer,
        config.buffer->buffer,
        config.offset,
        static_cast<VkIndexType>(config.index_format));
}

namespace GAL {
namespace {
void CmdBindDescriptorSets(
    Context ctx, CommandBuffer cmd_buffer,
    VkPipelineBindPoint bind_point,
    const DescriptorSetBindConfig& config
) {
    ctx->CmdBindDescriptorSets(
        cmd_buffer,
        bind_point,
        config.layout, config.first_set,
        config.sets.size(),
        config.sets.data(),
        config.dynamic_offsets.size(),
        config.dynamic_offsets.data());
}
}
}

void GAL::CmdBindGraphicsPipelineDescriptorSets(
    Context ctx, CommandBuffer cmd_buffer,
    const DescriptorSetBindConfig& config
) {
    CmdBindDescriptorSets(
        ctx, cmd_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, config);
}
}
