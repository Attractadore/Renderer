#pragma once
#if GAL_USE_VULKAN
#include "VulkanDescriptors.hpp"
#endif

#include "Buffer.hpp"
#include "Common/Flags.hpp"
#include "Context.hpp"
#include "PipelineStages.hpp"

namespace R1::GAL {
namespace Detail {
template<typename E>
concept IsDescriptorSetLayoutConfigOption = requires(E e) {
    E::PushDescriptor;
};

template<typename E>
concept IsDescriptorType = requires(E e) {
    E::Sampler;
    E::CombinedImageSampler;
    E::SampledImage;
    E::StorageImage;
    E::UniformTexelBuffer;
    E::StorageTexelBuffer;
    E::UniformBuffer;
    E::StorageBuffer;
    E::DynamicUniformBuffer;
    E::DynamicStorageBuffer;
    E::InputAttachment;
    E::InlineUniformBlock;
};

template<typename E>
concept IsDescriptorPoolConfigOption = requires(E e) {
    E::FreeDescriptorSet;
};

static_assert(IsDescriptorSetLayoutConfigOption<DescriptorSetLayoutConfigOption>);
static_assert(IsDescriptorType<DescriptorType>);
static_assert(IsDescriptorPoolConfigOption<DescriptorPoolConfigOption>);
}

using DescriptorSetLayoutConfigFlags    = Flags<DescriptorSetLayoutConfigOption>;
using DescriptorPoolConfigFlags         = Flags<DescriptorPoolConfigOption>;

struct DescriptorSetLayoutBinding {
    unsigned            binding;
    DescriptorType      type;
    unsigned            count;
    ShaderStageFlags    stages;
};

struct DescriptorSetLayoutConfig {
    DescriptorSetLayoutConfigFlags              flags;
    std::span<const DescriptorSetLayoutBinding> bindings;
};

DescriptorSetLayout CreateDescriptorSetLayout(
    Context ctx, const DescriptorSetLayoutConfig& config
);
void DestroyDescriptorSetLayout(Context ctx, DescriptorSetLayout layout);

struct DescriptorPoolSize {
    DescriptorType  type;
    unsigned        count;
};

struct DescriptorPoolConfig {
    DescriptorPoolConfigFlags           flags;
    unsigned                            set_count;
    std::span<const DescriptorPoolSize> pool_sizes;
};

DescriptorPool CreateDescriptorPool(
    Context ctx, const DescriptorPoolConfig& config
);
void DestroyDescriptorPool(Context ctx, DescriptorPool pool);
void ResetDescriptorPool(Context ctx, DescriptorPool pool);

enum class DescriptorSetAllocationResult {
    Success,
    FragmentedPool,
    OutOfPoolMemory,
};

struct DescriptorSetConfig {
    std::span<const GAL::DescriptorSetLayout> layouts;
};

DescriptorSetAllocationResult AllocateDescriptorSets(
    Context ctx, DescriptorPool pool,
    const DescriptorSetConfig& config,
    std::span<GAL::DescriptorSet> sets
);
void FreeDescriptorSets(
    Context ctx, DescriptorPool pool,
    std::span<GAL::DescriptorSet> sets
);

struct DescriptorBufferConfig {
    Buffer      buffer;
    size_t      offset;
    size_t      size;
};

struct DescriptorSetWriteConfig {
    DescriptorSet                           set;
    unsigned                                binding;
    unsigned                                first_index;
    DescriptorType                          type;
    std::span<const DescriptorBufferConfig> buffer_configs;
};

struct DescriptorSetCopyConfig {
    struct {
        DescriptorSet   set;
        unsigned        binding;
        unsigned        first_index;
    } src, dst;
    unsigned            count;
};

void UpdateDescriptorSets(
    Context ctx,
    std::span<const DescriptorSetWriteConfig> write_configs,
    std::span<const DescriptorSetCopyConfig> copy_configs
);
}
