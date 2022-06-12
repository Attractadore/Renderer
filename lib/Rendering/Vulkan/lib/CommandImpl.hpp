#pragma once
#include "Command.hpp"

namespace R1::VK {
constexpr VkMemoryBarrier2 MemoryBarrierToVK(
    const MemoryBarrier& barrier
);
VkBufferMemoryBarrier2 BufferBarrierToVK(
    const BufferBarrier& barrier
);
VkImageMemoryBarrier2 ImageBarrierToVK(
    const ImageBarrier& barrier
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
