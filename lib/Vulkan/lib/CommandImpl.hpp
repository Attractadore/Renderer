#pragma once
#include "Command.hpp"

namespace R1::VK {
VkCommandPoolCreateFlags CommandPoolCapabilitiesToVK(
    const CommandPoolCapabilities& caps
);

VkCommandPoolResetFlags CommandPoolResourcesToVK(
    CommandResources resources
);

VkCommandBufferResetFlags CommandBufferResourcesToVK(
    CommandResources resources
);

VkCommandBufferUsageFlags CommandBufferUsageToVK(
    const CommandBufferUsage& usg
);

constexpr VkPipelineStageFlags2 PipelineStagesToVK(
    const PipelineStages& stages
);
constexpr VkAccessFlags2 MemoryAccessesToVK(
    const MemoryAccesses& accesses
);

constexpr VkMemoryBarrier2 MemoryBarrierToVK(
    const MemoryBarrier& barrier
);
VkBufferMemoryBarrier2 BufferBarrierToVK(
    const BufferBarrier& barrier
);
VkImageMemoryBarrier2 ImageBarrierToVK(
    const ImageBarrier& barrier
);

constexpr VkRenderingFlags RenderingContinuationToVK(
    const RenderingContinuation& continuation
);
constexpr VkResolveModeFlags ResolveModeToVK(
    const ResolveMode& mode
);
constexpr VkClearValue ClearValueToVK(
    const ClearValue clear_value
);
constexpr VkRenderingAttachmentInfo RenderingAttachmentToVK(
    const RenderingAttachment& attachment
);

constexpr VkViewport ViewportToVK(const Viewport& viewport);
constexpr VkRect2D Rect2DToVK(const Rect2D& rect);
}

#include "CommandImpl.inl"
