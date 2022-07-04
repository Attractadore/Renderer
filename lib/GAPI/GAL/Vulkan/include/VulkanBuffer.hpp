#pragma once
#include <vulkan/vulkan.h>

namespace R1::GAL {
enum class BufferUsage {
    TransferSRC     = VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
    TransferDST     = VK_BUFFER_USAGE_TRANSFER_DST_BIT,
    UniformTexel    = VK_BUFFER_USAGE_UNIFORM_TEXEL_BUFFER_BIT,
    StorageTexel    = VK_BUFFER_USAGE_STORAGE_TEXEL_BUFFER_BIT,
    Uniform         = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
    Storage         = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
    Index           = VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
    Vertex          = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
    Indirect        = VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT,
    DeviceAddress   = VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT,
};

struct VulkanBuffer;
using Buffer = VulkanBuffer*;
}
