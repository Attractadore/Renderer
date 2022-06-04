#include "CommandImpl.hpp"
#include "ContextImpl.hpp"
#include "VKUtil.hpp"

namespace R1::VK {
CommandPool CreateCommandPool(
    Context ctx, const CommandPoolConfig& config
) {
    VkCommandPoolCreateInfo create_info = {
        .sType = sType(create_info),
        .flags = CommandPoolCapabilitiesToVK(config.capabilities),
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
        ctx->device.get(), pool, CommandPoolResourcesToVK(resources));
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
        cmd_buffer, CommandBufferResourcesToVK(resources));
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
        .flags = CommandBufferUsageToVK(begin_config.usage),
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
}
