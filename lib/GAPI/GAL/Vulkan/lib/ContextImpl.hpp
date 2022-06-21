#pragma once
#include "VKDispatchTable.h"
#include "VKRAII.hpp"
#include "VulkanContext.hpp"
#include "VKContextDispatcher.hpp"

namespace R1::GAL {
struct ContextImpl: VulkanContextDispatcher<ContextImpl> {
    Vk::Device                  device;
    VkPhysicalDevice            adapter;
    Vk::Allocator               allocator;
    VulkanDeviceDispatchTable   vk;

    const VulkanDeviceDispatchTable& GetDispatchTable() const noexcept { return vk; }
    VkDevice GetDevice() const noexcept { return device.get(); }
};
}
