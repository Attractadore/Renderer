#pragma once
#include <vulkan/vulkan.h>

namespace R1::GAL {
enum class DescriptorSetLayoutConfigOption {
    PushDescriptor = VK_DESCRIPTOR_SET_LAYOUT_CREATE_PUSH_DESCRIPTOR_BIT_KHR,
};

enum class DescriptorType {
    Sampler                 = VK_DESCRIPTOR_TYPE_SAMPLER,
    CombinedImageSampler    = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
    SampledImage            = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,
    StorageImage            = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,
    UniformTexelBuffer      = VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER,
    StorageTexelBuffer      = VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER,
    UniformBuffer           = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
    StorageBuffer           = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
    DynamicUniformBuffer    = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC,
    DynamicStorageBuffer    = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC,
    InputAttachment         = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT,
    InlineUniformBlock      = VK_DESCRIPTOR_TYPE_INLINE_UNIFORM_BLOCK,
};

enum class DescriptorPoolConfigOption {
    FreeDescriptorSet = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT,
};

using DescriptorSetLayout   = VkDescriptorSetLayout;
using DescriptorPool        = VkDescriptorPool;
using DescriptorSet         = VkDescriptorSet;
}
