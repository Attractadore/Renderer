#pragma once
#include "GAL/GAL.hpp"

#include <cstring>
#include <concepts>
#include <utility>

namespace R1::GAPI {
template<typename T>
concept BufferUsageTraits =
    std::convertible_to<decltype(T::UsageFlags), GAL::BufferUsageFlags> and
    std::convertible_to<decltype(T::MemoryUsage), GAL::BufferMemoryUsage>;

template<typename Q>
concept DeletionQueue = requires (Q q, GAL::Buffer b) {
    q.push(b);
};

template<typename T, BufferUsageTraits UsageTraits, DeletionQueue Q>
class ExclusiveBufferAllocator {
    static constexpr auto UsageFlags    = UsageTraits::UsageFlags;
    static constexpr auto MemoryUsage   = UsageTraits::MemoryUsage;
    GAL::Context    ctx;
    Q*              deletion_queue;

public:
    using value_type = T;

    ExclusiveBufferAllocator(GAL::Context c, Q* q) noexcept:
        ctx{c}, deletion_queue{q} {}

    bool operator==(const ExclusiveBufferAllocator& other) const noexcept = default;
    bool operator!=(const ExclusiveBufferAllocator& other) const noexcept = default;

    T* allocate(size_t cnt) {
        // TODO: don't store handle inside buffer
        auto buffer = GAL::CreateBuffer(ctx, {
            .size = sizeof(T) * cnt + sizeof(GAL::Buffer),
            .usage = UsageFlags,
            .memory_usage = MemoryUsage,
        });
        auto ptr = reinterpret_cast<T*>(GAL::GetBufferPointer(ctx, buffer));
        std::memcpy(ptr + cnt, &buffer, sizeof(buffer));
        return ptr;
    }

    void deallocate(T* ptr, size_t cnt) {
        if (ptr) {
            deletion_queue->push(get_backing_buffer(ptr, cnt));
        }
    }

    static GAL::Buffer get_backing_buffer(T* ptr, size_t cnt) {
        GAL::Buffer buffer;
        std::memcpy(&buffer, ptr + cnt, sizeof(buffer));
        return buffer;
    }

    using propagate_on_container_copy_assignment    = std::true_type;
    using propagate_on_container_move_assignment    = std::true_type;
    using propagate_on_container_swap               = std::true_type;
};
}
