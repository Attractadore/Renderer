#pragma once
#include "VKDispatchTable.h"
#include "VKRAII.hpp"
#include "VulkanContext.hpp"

namespace R1::GAL {
struct ContextImpl {
    Vk::Device                  device;
    VkPhysicalDevice            adapter;
    Vk::Allocator               allocator;
    VulkanDeviceDispatchTable   vk;
};
}
