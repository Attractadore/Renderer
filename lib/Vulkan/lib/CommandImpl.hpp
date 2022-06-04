#pragma once
#include "Command.hpp"

namespace R1::VK {
VkCommandPoolCreateFlags CommandPoolCapabilitiesToVK(
    const CommandPoolCapabilities& caps
);

VkCommandPoolResetFlags CommandPoolResourcesToVK(
    CommandResources resources
);

VkCommandBufferResetFlags CommandBufferResourcesToVK(
    CommandResources resources
);

VkCommandBufferUsageFlags CommandBufferUsageToVK(
    const CommandBufferUsage& usg
);
}

#include "CommandImpl.inl"
