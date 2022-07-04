#pragma once
#if GAL_USE_VULKAN
#include "VulkanBuffer.hpp"
#endif

#include "Common/Flags.hpp"
#include "Context.hpp"

namespace R1::GAL {
namespace Detail {
template<typename E>
concept IsBufferUsage = requires(E e) {
    E::TransferSRC;
    E::TransferDST;
    E::UniformTexel;
    E::StorageTexel;
    E::Uniform;
    E::Storage;
    E::Index;
    E::Vertex;
    E::Indirect;
    E::DeviceAddress;
};

static_assert(IsBufferUsage<BufferUsage>);
}

using BufferUsageFlags = Flags<BufferUsage>;

enum class BufferMemoryUsage {
    // Device local, not mappable
    Device,
    // Host visible, not cached
    Staging,
    // Host visible, cached
    Readback,
    // Device and host visible, not cached, prefer device local
    Streaming,
};

enum class BufferMemoryPropery {
    Coherant,
};

using BufferMemoryProperyFlags = Flags<BufferMemoryPropery>;

struct BufferConfig {
    size_t                              size;
    BufferUsageFlags                    usage;
    std::span<const QueueFamily::ID>    sharing_queue_families;
    BufferMemoryUsage                   memory_usage;
};

Buffer CreateBuffer(Context ctx, const BufferConfig& config);
void DestroyBuffer(Context ctx, Buffer buffer);

BufferMemoryProperyFlags GetBufferMemoryProperties(Context ctx, Buffer buffer);
void* GetBufferPointer(Context ctx, Buffer buffer);
void FlushBufferRange(Context ctx, Buffer buffer, size_t offset, size_t size);
void InvalidateBufferRange(Context ctx, Buffer buffer, size_t offset, size_t size);
}
