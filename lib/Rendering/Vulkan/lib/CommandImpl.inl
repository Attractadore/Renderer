#pragma once
#include "ImageImpl.hpp"
#include "VKUtil.hpp"

namespace R1::VK {
inline VkCommandPoolCreateFlags CommandPoolCapabilitiesToVK(
    const CommandPoolCapabilities& caps
) {
    VkCommandPoolCreateFlags flags = 0;
    flags |= caps.transient ? VK_COMMAND_POOL_CREATE_TRANSIENT_BIT: 0;
    flags |= caps.reset_command_buffer ? VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT: 0;
    return flags;
}

inline VkCommandPoolResetFlags CommandPoolResourcesToVK(
    CommandResources resources
) {
    switch(resources) {
        case CommandResources::Release:
            return VK_COMMAND_POOL_RESET_RELEASE_RESOURCES_BIT;
        default:
            return 0;
    }
}

inline VkCommandBufferResetFlags CommandBufferResourcesToVK(
    CommandResources resources
) {
    switch(resources) {
        case CommandResources::Release:
            return VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT;
        default:
            return 0;
    }
}

inline VkCommandBufferUsageFlags CommandBufferUsageToVK(
    const CommandBufferUsage& usg
) {
    VkCommandBufferUsageFlags flags = 0;
    flags |= usg.one_time_submit ? VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT: 0;
    flags |= usg.simultaneous_use ? VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT: 0;
    return flags;
}

constexpr inline VkPipelineStageFlags2 PipelineStagesToVK(const PipelineStages& stages) {
    VkPipelineStageFlags2 flags = 0;
    flags |= stages.draw_indirect ? VK_PIPELINE_STAGE_2_DRAW_INDIRECT_BIT: 0;
    flags |= stages.index_input ? VK_PIPELINE_STAGE_2_INDEX_INPUT_BIT: 0;
    flags |= stages.vertex_attribute_input ? VK_PIPELINE_STAGE_2_VERTEX_ATTRIBUTE_INPUT_BIT: 0;
    flags |= stages.vertex_input ? VK_PIPELINE_STAGE_2_VERTEX_INPUT_BIT: 0;
    flags |= stages.vertex_shader ? VK_PIPELINE_STAGE_2_VERTEX_SHADER_BIT: 0;
    flags |= stages.tessellation_control_shader ? VK_PIPELINE_STAGE_2_TESSELLATION_CONTROL_SHADER_BIT: 0;
    flags |= stages.tessellation_evaluation_shader ? VK_PIPELINE_STAGE_2_TESSELLATION_EVALUATION_SHADER_BIT: 0;
    flags |= stages.geometry_shader ? VK_PIPELINE_STAGE_2_GEOMETRY_SHADER_BIT: 0;
    flags |= stages.pre_rasterization_shaders ? VK_PIPELINE_STAGE_2_PRE_RASTERIZATION_SHADERS_BIT: 0;
    flags |= stages.fragment_shader ? VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT: 0;
    flags |= stages.early_fragment_tests ? VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT: 0;
    flags |= stages.late_fragment_tests ? VK_PIPELINE_STAGE_2_LATE_FRAGMENT_TESTS_BIT: 0;
    flags |= stages.color_attachment_output ? VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT: 0;
    flags |= stages.all_graphics ? VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT: 0;
    flags |= stages.copy ? VK_PIPELINE_STAGE_2_COPY_BIT: 0;
    flags |= stages.resolve ? VK_PIPELINE_STAGE_2_RESOLVE_BIT: 0;
    flags |= stages.blit ? VK_PIPELINE_STAGE_2_BLIT_BIT: 0;
    flags |= stages.clear ? VK_PIPELINE_STAGE_2_CLEAR_BIT: 0;
    flags |= stages.all_transfer ? VK_PIPELINE_STAGE_2_ALL_TRANSFER_BIT: 0;
    flags |= stages.compute_shader ? VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT: 0;
    flags |= stages.all_commands ? VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT: 0;
    return flags;
}

constexpr inline VkAccessFlags2 MemoryAccessesToVK(const MemoryAccesses& accesses) {
    VkAccessFlags2 flags = 0;
    flags |= accesses.indirect_command_read ? VK_ACCESS_2_INDIRECT_COMMAND_READ_BIT: 0;
    flags |= accesses.index_read ? VK_ACCESS_2_INDEX_READ_BIT: 0;
    flags |= accesses.vertex_attribute_read ? VK_ACCESS_2_VERTEX_ATTRIBUTE_READ_BIT: 0;
    flags |= accesses.uniform_read ? VK_ACCESS_2_UNIFORM_READ_BIT: 0;
    flags |= accesses.shader_sampled_read ? VK_ACCESS_2_SHADER_SAMPLED_READ_BIT: 0;
    flags |= accesses.shader_storage_read ? VK_ACCESS_2_SHADER_STORAGE_READ_BIT: 0;
    flags |= accesses.shader_read ? VK_ACCESS_2_SHADER_READ_BIT: 0;
    flags |= accesses.shader_storage_write ? VK_ACCESS_2_SHADER_STORAGE_WRITE_BIT: 0;
    flags |= accesses.shader_write ? VK_ACCESS_2_SHADER_WRITE_BIT: 0;
    flags |= accesses.input_attachment_read ? VK_ACCESS_2_INPUT_ATTACHMENT_READ_BIT: 0;
    flags |= accesses.color_attachment_read ? VK_ACCESS_2_COLOR_ATTACHMENT_READ_BIT: 0;
    flags |= accesses.color_attachment_write ? VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT: 0;
    flags |= accesses.depth_stencil_attachment_read ? VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_READ_BIT: 0;
    flags |= accesses.depth_stencil_attachment_write ? VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT: 0;
    flags |= accesses.transfer_read ? VK_ACCESS_2_TRANSFER_READ_BIT: 0;
    flags |= accesses.transfer_write ? VK_ACCESS_2_TRANSFER_WRITE_BIT: 0;
    flags |= accesses.memory_read ? VK_ACCESS_2_MEMORY_READ_BIT: 0;
    flags |= accesses.memory_write ? VK_ACCESS_2_MEMORY_WRITE_BIT: 0;
    return flags;
}

inline constexpr VkMemoryBarrier2 MemoryBarrierToVK(
    const MemoryBarrier& barrier
) {
    VkMemoryBarrier2 bar = {
        .sType = sType(bar),
        .srcStageMask = PipelineStagesToVK(barrier.src_stages),
        .srcAccessMask = MemoryAccessesToVK(barrier.src_accesses),
        .dstStageMask = PipelineStagesToVK(barrier.dst_stages),
        .dstAccessMask = MemoryAccessesToVK(barrier.dst_accesses),
    };
    return bar;
}

inline VkBufferMemoryBarrier2 BufferBarrierToVK(
    const BufferBarrier& barrier
) {
    VkBufferMemoryBarrier2 bar = {
        .sType = sType(bar),
        .srcStageMask = PipelineStagesToVK(barrier.memory_barrier.src_stages),
        .srcAccessMask = MemoryAccessesToVK(barrier.memory_barrier.src_accesses),
        .dstStageMask = PipelineStagesToVK(barrier.memory_barrier.dst_stages),
        .dstAccessMask = MemoryAccessesToVK(barrier.memory_barrier.dst_accesses),
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
        .srcStageMask = PipelineStagesToVK(barrier.memory_barrier.src_stages),
        .srcAccessMask = MemoryAccessesToVK(barrier.memory_barrier.src_accesses),
        .dstStageMask = PipelineStagesToVK(barrier.memory_barrier.dst_stages),
        .dstAccessMask = MemoryAccessesToVK(barrier.memory_barrier.dst_accesses),
        .oldLayout = static_cast<VkImageLayout>(barrier.old_layout),
        .newLayout = static_cast<VkImageLayout>(barrier.new_layout),
        .srcQueueFamilyIndex = static_cast<uint32_t>(barrier.queue_family_transfer.src),
        .dstQueueFamilyIndex = static_cast<uint32_t>(barrier.queue_family_transfer.dst),
        .image = barrier.image->image.get(),
        .subresourceRange = ImageSubresourceRangeToVK(barrier.subresource_range),
    };
    return bar;
}

inline constexpr VkRenderingFlags RenderingContinuationToVK(
    const RenderingContinuation& continuation
) {
    VkRenderingFlags flags = 0;
    flags |= continuation.resume ? VK_RENDERING_RESUMING_BIT: 0;
    flags |= continuation.suspend ? VK_RENDERING_SUSPENDING_BIT: 0;
    return flags;
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
