#pragma once
#include <vulkan/vulkan.h>

namespace R1::GAL {
enum class CommandPoolConfigOption {
    Transient               = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT,
    AllowCommandBufferReset = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
};

enum class CommandResources {
    Keep = 0,
    Release = VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT,
};

enum class CommandBufferUsage {
    OneTimeSubmit   = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
    Simultaneous    = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT,
};

enum class ResolveMode {
    None        = VK_RESOLVE_MODE_NONE,
    SampleZero  = VK_RESOLVE_MODE_SAMPLE_ZERO_BIT,
    Average     = VK_RESOLVE_MODE_AVERAGE_BIT,
    Min         = VK_RESOLVE_MODE_MIN_BIT,
    Max         = VK_RESOLVE_MODE_MAX_BIT,
};

enum class AttachmentLoadOp {
    Load        = VK_ATTACHMENT_LOAD_OP_LOAD,
    Clear       = VK_ATTACHMENT_LOAD_OP_CLEAR,
    DontCare    = VK_ATTACHMENT_LOAD_OP_DONT_CARE, 
};

enum class AttachmentStoreOp {
    Store       = VK_ATTACHMENT_STORE_OP_STORE,
    DontCare    = VK_ATTACHMENT_STORE_OP_DONT_CARE,
    None        = VK_ATTACHMENT_STORE_OP_NONE,
};

enum class RenderingConfigOption {
    Resume = VK_RENDERING_RESUMING_BIT_KHR,
    Suspend = VK_RENDERING_SUSPENDING_BIT,
};

enum class IndexFormat {
    U16 = VK_INDEX_TYPE_UINT16,
    U32 = VK_INDEX_TYPE_UINT32,
};

using CommandPool = VkCommandPool;
using CommandBuffer = VkCommandBuffer;
}
