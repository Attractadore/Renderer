#include "BufferImpl.hpp"
#include "Common/Vector.hpp"
#include "ContextImpl.hpp"
#include "GAL/Descriptors.hpp"
#include "VKUtil.hpp"

#include <algorithm>

namespace R1 {
GAL::DescriptorSetLayout GAL::CreateDescriptorSetLayout(
    Context ctx, const DescriptorSetLayoutConfig& config
) {
    DefaultSmallVector<VkDescriptorSetLayoutBinding> bindings;
    bindings.resize(config.bindings.size());
    std::ranges::transform(config.bindings, bindings.begin(),
        [] (const DescriptorSetLayoutBinding& binding) {
            return VkDescriptorSetLayoutBinding {
                .binding = binding.binding,
                .descriptorType = static_cast<VkDescriptorType>(binding.type),
                .descriptorCount = binding.count,
                .stageFlags = static_cast<VkShaderStageFlags>(binding.stages.Extract()),
            };
        });
    VkDescriptorSetLayoutCreateInfo create_info = {
        .sType = SType(create_info),
        .flags = static_cast<VkDescriptorSetLayoutCreateFlags>(config.flags.Extract()),
        .bindingCount = static_cast<uint32_t>(bindings.size()),
        .pBindings = bindings.data(),
    };
    VkDescriptorSetLayout layout;
    ThrowIfFailed(
        ctx->CreateDescriptorSetLayout(&create_info, &layout),
        "Vulkan: Failed to create descriptor set layout");
    return layout;
}

void GAL::DestroyDescriptorSetLayout(Context ctx, DescriptorSetLayout layout) {
    ctx->DestroyDescriptorSetLayout(layout);
}

GAL::DescriptorPool GAL::CreateDescriptorPool(
    Context ctx, const DescriptorPoolConfig& config
) {
    DefaultSmallVector<VkDescriptorPoolSize> pool_sizes;
    pool_sizes.resize(config.pool_sizes.size());
    std::ranges::transform(config.pool_sizes, pool_sizes.begin(),
        [] (const DescriptorPoolSize& pool_size) {
            return VkDescriptorPoolSize {
                .type = static_cast<VkDescriptorType>(pool_size.type),
                .descriptorCount = pool_size.count,
            };
        });
    VkDescriptorPoolCreateInfo create_info = {
        .sType = SType(create_info),
        .flags = static_cast<VkDescriptorPoolCreateFlags>(config.flags.Extract()),
        .maxSets = config.set_count,
        .poolSizeCount = static_cast<uint32_t>(pool_sizes.size()),
        .pPoolSizes = pool_sizes.data(),
    };
    VkDescriptorPool pool;
    ThrowIfFailed(
        ctx->CreateDescriptorPool(&create_info, &pool),
        "Vulkan: Failed to create descriptor pool");
    return pool;
}

void GAL::DestroyDescriptorPool(Context ctx, DescriptorPool pool) {
    ctx->DestroyDescriptorPool(pool);
}

void GAL::ResetDescriptorPool(Context ctx, DescriptorPool pool) {
    ctx->ResetDescriptorPool(pool, 0);
}

GAL::DescriptorSetAllocationResult GAL::AllocateDescriptorSets(
    Context ctx, DescriptorPool pool,
    const DescriptorSetConfig& config,
    std::span<GAL::DescriptorSet> sets
) {
    assert(config.layouts.size() == sets.size());
    VkDescriptorSetAllocateInfo alloc_info = {
        .sType = SType(alloc_info),
        .descriptorPool = pool,
        .descriptorSetCount = static_cast<uint32_t>(sets.size()),
        .pSetLayouts = config.layouts.data(),
    };
    auto r = ctx->AllocateDescriptorSets(&alloc_info, sets.data());
    using enum DescriptorSetAllocationResult;
    switch(r) {
        case VK_SUCCESS:
            return Success;
        case VK_ERROR_FRAGMENTED_POOL:
            return FragmentedPool;
        case VK_ERROR_OUT_OF_POOL_MEMORY:
            return OutOfPoolMemory;
        default:
            throw std::runtime_error{
                "Vulkan: Failed to allocate descriptor sets"};
    }
}

void GAL::FreeDescriptorSets(
    Context ctx, DescriptorPool pool,
    std::span<GAL::DescriptorSet> sets
) {
    ctx->FreeDescriptorSets(pool, sets.size(), sets.data());
}

void GAL::UpdateDescriptorSets(
    Context ctx,
    std::span<const DescriptorSetWriteConfig> write_configs,
    std::span<const DescriptorSetCopyConfig> copy_configs
) {
    DefaultSmallVector<VkDescriptorBufferInfo> buffer_infos;
    DefaultSmallVector<VkWriteDescriptorSet> writes;
    DefaultSmallVector<VkCopyDescriptorSet> copies;

    buffer_infos.reserve(
        ranges::accumulate(write_configs, 0, std::plus{},
        [] (const DescriptorSetWriteConfig& config) {
            return config.buffer_configs.size();
        })
    );

    writes.resize(write_configs.size());
    std::ranges::transform(write_configs, writes.begin(),
        [&buffer_infos] (const DescriptorSetWriteConfig& config) {
            auto old_data = buffer_infos.data();
            auto v = ranges::views::transform(config.buffer_configs,
                [] (const DescriptorBufferConfig& config) {
                    return VkDescriptorBufferInfo {
                        .buffer = config.buffer->buffer,
                        .offset = config.offset,
                        .range = config.size,
                    };
                });
            auto it = VecAppend(buffer_infos, v);
            assert(old_data == buffer_infos.data());
            VkWriteDescriptorSet write = {
               .sType = SType(write),
               .dstSet = config.set,
               .dstBinding = config.binding,
               .dstArrayElement = config.first_index,
               .descriptorCount = static_cast<uint32_t>(config.buffer_configs.size()),
               .descriptorType = static_cast<VkDescriptorType>(config.type),
               .pBufferInfo = &*it,
            };
            return write;
        });

    copies.resize(copy_configs.size());
    std::ranges::transform(copy_configs, copies.begin(),
        [] (const DescriptorSetCopyConfig& config) {
            VkCopyDescriptorSet copy = {
                .sType = SType(copy),
                .srcSet = config.src.set,
                .srcBinding = config.src.binding,
                .srcArrayElement = config.src.first_index,
                .dstSet = config.dst.set,
                .dstBinding = config.dst.binding,
                .dstArrayElement = config.dst.first_index,
                .descriptorCount = config.count,
            };
            return copy;
        });

    ctx->UpdateDescriptorSets(
        writes.size(), writes.data(),
        copies.size(), copies.data());
}
}
