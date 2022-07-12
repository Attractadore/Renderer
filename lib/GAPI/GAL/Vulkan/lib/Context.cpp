#include "Common/Vector.hpp"
#include "ContextImpl.hpp"
#include "DeviceImpl.hpp"
#include "GAL/Context.hpp"
#include "VKUtil.hpp"
#include "VulkanContext.inl"

namespace R1::GAL {
namespace {
VkDevice CreateDevice(
    Device parent,
    const ContextConfig& config,
    const VkDeviceCreateInfo* create_template
) {
    const auto& dev_desc = parent->description.common;

    float priority = 1.0f;
    auto v = ranges::views::transform(config.queue_config, [&](const auto& qcfg) {
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

    VkPhysicalDeviceVulkan12Features vulkan12_features = {
        .sType = SType(vulkan12_features),
        .scalarBlockLayout = true,
        .timelineSemaphore = true,
    };

    VkPhysicalDeviceVulkan13Features vulkan13_features = {
        .sType = SType(vulkan13_features),
        .pNext = &vulkan12_features,
        .synchronization2 = true,
        .dynamicRendering = true,
    };

    // TODO: merge user features with required features
    VkDeviceCreateInfo create_info = {
        .sType = SType(create_info),
        .pNext = &vulkan13_features,
        .queueCreateInfoCount =
            static_cast<uint32_t>(queue_create_infos.size()),
        .pQueueCreateInfos = queue_create_infos.data(),
        .enabledExtensionCount =
            create_template->enabledExtensionCount,
        .ppEnabledExtensionNames =
            create_template->ppEnabledExtensionNames,
    };

    VkDevice dev;
    ThrowIfFailed(
        vkCreateDevice(parent->physical_device, &create_info, nullptr, &dev),
        "Vulkan: Failed to create context");

    return dev;
}

void CreateContextDevice(
    Context ctx,
    Device parent,
    const ContextConfig& config,
    const VkDeviceCreateInfo* create_template
) {
    ctx->device.reset(CreateDevice(parent, config, create_template));
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

Context Vulkan::CreateContextFromTemplate(
    Device parent,
    const ContextConfig& config,
    const VkDeviceCreateInfo* create_template
) {
    auto ctx = std::make_unique<ContextImpl>(ContextImpl{
        .adapter = parent->physical_device,
    });
    CreateContextDevice(ctx.get(), parent, config, create_template);
    CreateContextAllocator(ctx.get(), parent);
    return ctx.release();
}

VkDevice Vulkan::GetVkDevice(Context ctx) {
    return ctx->device.get();
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
