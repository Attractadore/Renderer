#pragma once
#include "ImageImpl.hpp"
#include "VKUtil.hpp"

namespace R1::GAL {
inline constexpr VkMemoryBarrier2 MemoryBarrierToVK(
    const MemoryBarrier& barrier
) {
    VkMemoryBarrier2 bar = {
        .sType = sType(bar),
        .srcStageMask = static_cast<VkPipelineStageFlags2>(
            barrier.src_stages.Extract()),
        .srcAccessMask = static_cast<VkPipelineStageFlags2>(
            barrier.src_accesses.Extract()),
        .dstStageMask = static_cast<VkPipelineStageFlags2>(
            barrier.dst_stages.Extract()),
        .dstAccessMask = static_cast<VkPipelineStageFlags2>(
            barrier.dst_accesses.Extract()),
    };
    return bar;
}

inline VkBufferMemoryBarrier2 BufferBarrierToVK(
    const BufferBarrier& barrier
) {
    VkBufferMemoryBarrier2 bar = {
        .sType = sType(bar),
        .srcStageMask = static_cast<VkPipelineStageFlags2>(
            barrier.memory_barrier.src_stages.Extract()),
        .srcAccessMask = static_cast<VkPipelineStageFlags2>(
            barrier.memory_barrier.src_accesses.Extract()),
        .dstStageMask = static_cast<VkPipelineStageFlags2>(
            barrier.memory_barrier.dst_stages.Extract()),
        .dstAccessMask = static_cast<VkPipelineStageFlags2>(
            barrier.memory_barrier.dst_accesses.Extract()),
        .srcQueueFamilyIndex = static_cast<uint32_t>(barrier.queue_family_transfer.src),
        .dstQueueFamilyIndex = static_cast<uint32_t>(barrier.queue_family_transfer.dst),
        .offset = barrier.offset,
        .size = barrier.size,
    };
    return bar;
}

inline VkImageMemoryBarrier2 ImageBarrierToVK(
    const ImageBarrier& barrier
) {
    VkImageMemoryBarrier2 bar = {
        .sType = sType(bar),
        .srcStageMask = static_cast<VkPipelineStageFlags2>(
            barrier.memory_barrier.src_stages.Extract()),
        .srcAccessMask = static_cast<VkPipelineStageFlags2>(
            barrier.memory_barrier.src_accesses.Extract()),
        .dstStageMask = static_cast<VkPipelineStageFlags2>(
            barrier.memory_barrier.dst_stages.Extract()),
        .dstAccessMask = static_cast<VkPipelineStageFlags2>(
            barrier.memory_barrier.dst_accesses.Extract()),
        .oldLayout = static_cast<VkImageLayout>(barrier.old_layout),
        .newLayout = static_cast<VkImageLayout>(barrier.new_layout),
        .srcQueueFamilyIndex = static_cast<uint32_t>(barrier.queue_family_transfer.src),
        .dstQueueFamilyIndex = static_cast<uint32_t>(barrier.queue_family_transfer.dst),
        .image = barrier.image->image,
        .subresourceRange = ImageSubresourceRangeToVK(barrier.subresource_range),
    };
    return bar;
}

inline constexpr VkClearValue ClearValueToVK(
    const ClearValue clear_value
) {
    return VkClearValue {
        .color = {
            .float32 = {
                clear_value.color.r,
                clear_value.color.g,
                clear_value.color.b,
                clear_value.color.a,
            }
        }
    };
}

inline constexpr VkRenderingAttachmentInfo RenderingAttachmentToVK(
    const RenderingAttachment& attachment
) {
    VkRenderingAttachmentInfo vk_attachment = {
        .sType = sType(vk_attachment),
        .imageView = attachment.view,
        .imageLayout = static_cast<VkImageLayout>(attachment.layout),
        .resolveMode = static_cast<VkResolveModeFlagBits>(attachment.resolve.mode),
        .resolveImageView = attachment.resolve.view,
        .resolveImageLayout = static_cast<VkImageLayout>(attachment.resolve.layout),
        .loadOp = static_cast<VkAttachmentLoadOp>(attachment.load_op),
        .storeOp = static_cast<VkAttachmentStoreOp>(attachment.store_op),
        .clearValue = ClearValueToVK(attachment.clear_value),
    };
    return vk_attachment;
}

inline constexpr VkViewport ViewportToVK(const Viewport& viewport) {
    return VkViewport {
        .x = viewport.x,
        .y = viewport.y,
        .width = viewport.width,
        .height = viewport.height,
        .minDepth = viewport.min_depth,
        .maxDepth = viewport.max_depth,
    };
}

inline constexpr VkRect2D Rect2DToVK(const Rect2D& rect) {
    return VkRect2D {
        .offset = {
            .x = rect.x,
            .y = rect.y,
        },
        .extent = {
            .width = rect.width,
            .height = rect.height,
        },
    };
}
}
