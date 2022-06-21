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
    ThrowIfFailed(
        vkCreateDevice(parent->physical_device, &create_info, nullptr, &dev),
        "Vulkan: Failed to create context");

    return Vk::Device{dev};
}

void CreateContextDevice(
    Context ctx, const ContextConfig& config, Device parent
) {
    ctx->device = CreateDevice(config, parent);
    LoadVulkanDeviceDispatchTable(&ctx->vk, ctx->device.get());
}

void CreateContextAllocator(Context ctx, Device parent) {
    VmaVulkanFunctions funcs = {
        .vkGetInstanceProcAddr = vkGetInstanceProcAddr,
        .vkGetDeviceProcAddr = vkGetDeviceProcAddr,
    };
    VmaAllocatorCreateInfo allocatorCreateInfo = {
        .physicalDevice = parent->physical_device,
        .device = ctx->device.get(),
        .pAllocationCallbacks = ctx->GetAllocationCallbacks(),
        .pVulkanFunctions = &funcs,
        .instance = parent->instance,
        .vulkanApiVersion = parent->description.api_version,
    };
    VmaAllocator allocator;
    ThrowIfFailed(
        vmaCreateAllocator(&allocatorCreateInfo, &allocator),
        "Vulkan: Failed to create context");
    ctx->allocator.reset(allocator);
}
}

Context CreateContext(Device parent, const ContextConfig& config) {
    auto ctx = std::make_unique<ContextImpl>(ContextImpl{
        .adapter = parent->physical_device,
    });
    CreateContextDevice(ctx.get(), config, parent);
    CreateContextAllocator(ctx.get(), parent);
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
