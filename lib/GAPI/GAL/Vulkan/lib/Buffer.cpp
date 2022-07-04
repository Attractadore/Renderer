#include "BufferImpl.hpp"
#include "ContextImpl.hpp"
#include "VKUtil.hpp"

#include <memory>

namespace R1::GAL {
Buffer CreateBuffer(Context ctx, const BufferConfig& config) {
    VkBufferCreateInfo create_info = {
        .sType = SType(create_info),
        .size = config.size,
        .usage =
            static_cast<VkBufferUsageFlags>(config.usage.Extract()),
        .sharingMode = config.sharing_queue_families.empty() ?
            VK_SHARING_MODE_EXCLUSIVE: VK_SHARING_MODE_CONCURRENT,
        .queueFamilyIndexCount =
            static_cast<uint32_t>(config.sharing_queue_families.size()),
        .pQueueFamilyIndices =
            reinterpret_cast<const uint32_t*>(config.sharing_queue_families.data()),
    };

    auto [alloc_usg, alloc_flags] = [] (BufferMemoryUsage mem_usg) ->
        std::tuple<VmaMemoryUsage, VmaAllocationCreateFlags> {
    using enum BufferMemoryUsage;
    switch(mem_usg) {
        case Device:
            return { VMA_MEMORY_USAGE_AUTO, 0 };
        case Staging:
            return { VMA_MEMORY_USAGE_AUTO_PREFER_HOST,
                VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT |
                VMA_ALLOCATION_CREATE_MAPPED_BIT };
        case Readback:
            return { VMA_MEMORY_USAGE_AUTO,
                VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT |
                VMA_ALLOCATION_CREATE_MAPPED_BIT };
        case Streaming:
            return { VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE,
                VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT |
                VMA_ALLOCATION_CREATE_MAPPED_BIT };
        default:
            assert(!"Unknow buffer memory usage");
    }} (config.memory_usage);

    VmaAllocationCreateInfo alloc_info = {
        .flags = alloc_flags,
        .usage = alloc_usg,
    };

    auto buffer = std::make_unique<VulkanBuffer>();
    ThrowIfFailed(vmaCreateBuffer(
        ctx->allocator.get(),
        &create_info, &alloc_info,
        &buffer->buffer, &buffer->allocation, nullptr),
        "Vulkan: Failed to create buffer");
    return buffer.release();
}

void DestroyBuffer(Context ctx, Buffer buffer) {
    vmaDestroyBuffer(ctx->allocator.get(),
        buffer->buffer, buffer->allocation);
}

void* GetBufferPointer(Context ctx, Buffer buffer) {
    VmaAllocationInfo alloc_info;
    vmaGetAllocationInfo(ctx->allocator.get(), buffer->allocation, &alloc_info);
    return alloc_info.pMappedData;
}

void FlushBufferRange(Context ctx, Buffer buffer, size_t offset, size_t size) {
    ThrowIfFailed(vmaFlushAllocation(
        ctx->allocator.get(), buffer->allocation, offset, size),
        "Vulkan: Failed to flush buffer");
}
}
