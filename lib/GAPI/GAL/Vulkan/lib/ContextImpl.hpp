#pragma once
#include "VKContextDispatcher.hpp"
#include "VKDispatchTable.h"
#include "VKRAII.hpp"
#include "VulkanContext.hpp"

namespace R1::GAL {
struct ContextImpl: VulkanContextDispatcher<ContextImpl> {
    Vk::Device                  device;
    VkPhysicalDevice            adapter;
    Vk::Allocator               allocator;
    VulkanDeviceDispatchTable   vk;

    constexpr const VulkanDeviceDispatchTable& GetDispatchTable() const noexcept { return vk; }
    VkDevice GetDevice() const noexcept { return device.get(); }
    constexpr const VkAllocationCallbacks* GetAllocationCallbacks() const noexcept { return nullptr; }
};
}
