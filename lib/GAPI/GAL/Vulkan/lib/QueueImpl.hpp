#pragma once
#include "GAL/Queue.hpp"

namespace R1::GAL {
VkSemaphoreSubmitInfo SemaphoreSubmitToVK(const SemaphoreSubmitConfig& config);
VkCommandBufferSubmitInfo CommandBufferSubmitToVK(CommandBuffer cmd_buffer);
}
