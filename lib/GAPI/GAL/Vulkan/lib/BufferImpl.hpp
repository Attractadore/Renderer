#pragma once
#include "GAL/Buffer.hpp"

#include <vk_mem_alloc.h>

namespace R1::GAL {
struct VulkanBuffer {
    VkBuffer        buffer;
    VmaAllocation   allocation;
};
}
