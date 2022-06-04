#pragma once

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
}
