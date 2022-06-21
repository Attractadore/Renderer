#include "Common/Vector.hpp"
#include "ContextImpl.hpp"
#include "DeviceImpl.hpp"
#include "GAL/Context.hpp"
#include "VKUtil.hpp"

namespace R1::GAL {
namespace {
Vk::Device CreateDevice(
    const ContextConfig& config, Device parent 
) {
    const auto& dev_desc = parent->description.common;

    float priority = 1.0f;
    auto v = std::views::transform(config.queue_config, [&](const auto& qcfg) {
        assert(static_cast<size_t>(qcfg.id) <= dev_desc.queue_families.size());
        VkDeviceQueueCreateInfo create_info = {
            .sType = SType(create_info),
            .queueFamilyIndex =
                static_cast<uint32_t>(qcfg.id),
            .queueCount = qcfg.count,
            .pQueuePriorities = &priority,
        };
        return create_info;
    });
    auto queue_create_infos = vec_from_range(v);

    std::vector<const char*> exts;
    if (config.wsi) {
        assert(dev_desc.wsi);
        exts.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
    }

    VkPhysicalDeviceTimelineSemaphoreFeatures timeline_semaphore_features = {
        .sType = SType(timeline_semaphore_features),
        .timelineSemaphore = true,
    };

    VkPhysicalDeviceVulkan13Features vulkan13_features = {
        .sType = SType(vulkan13_features),
        .pNext = &timeline_semaphore_features,
        .synchronization2 = true,
        .dynamicRendering = true,
    };
    
    VkDeviceCreateInfo create_info = {
        .sType = SType(create_info),
        .pNext = &vulkan13_features,
        .queueCreateInfoCount =
            static_cast<uint32_t>(queue_create_infos.size()),
        .pQueueCreateInfos = queue_create_infos.data(),
        .enabledExtensionCount =
            static_cast<uint32_t>(exts.size()),
        .ppEnabledExtensionNames = exts.data(),
    };

    VkDevice dev = VK_NULL_HANDLE;
    vkCreateDevice(parent->physical_device, &create_info, nullptr, &dev);

    return Vk::Device{dev};
}

Vk::Allocator CreateAllocator(VkDevice device, Device parent) {
    VmaAllocatorCreateInfo allocatorCreateInfo = {
        .physicalDevice = parent->physical_device,
        .device = device,
        .instance = parent->instance,
        .vulkanApiVersion = parent->description.api_version,
    };

    VmaAllocator allocator = nullptr;
    vmaCreateAllocator(&allocatorCreateInfo, &allocator);

    return Vk::Allocator{allocator};
}
}

Context CreateContext(Device parent, const ContextConfig& config) {
    auto dev = CreateDevice(config, parent);
    if (!dev) {
        throw std::runtime_error{"Vulkan: Failed to create context"};
    }
    auto alloc = CreateAllocator(dev.get(), parent);
    if (!alloc) {
        throw std::runtime_error{"Vulkan: Failed to create context"};
    }
    auto ctx = std::make_unique<ContextImpl>(ContextImpl{
        .device = std::move(dev),
        .adapter = parent->physical_device,
        .allocator = std::move(alloc),
    });
    LoadVulkanDeviceDispatchTable(&ctx->vk, ctx->device.get());
    return ctx.release();
}

void DestroyContext(Context ctx) {
    delete ctx;
}

void ContextWaitIdle(Context ctx) {
    ThrowIfFailed(
        ctx->DeviceWaitIdle(),
        "Vulkan: Failed to wait for idle context");
}
}
