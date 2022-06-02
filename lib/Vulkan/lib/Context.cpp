#include "Common/Vector.hpp"
#include "Context.hpp"
#include "VKUtil.hpp"

namespace R1::VK {
namespace {
Vk::Device CreateDevice(
    const ContextConfig& config, Device parent 
) {
    const auto& dev_desc = parent->description;

    float priority = 1.0f;
    auto v = std::views::transform(config.queue_config, [&](const auto& qcfg) {
        assert(static_cast<size_t>(qcfg.id) <= dev_desc.queue_families.size());
        VkDeviceQueueCreateInfo create_info = {
            .sType = sType(create_info),
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

    VkPhysicalDeviceVulkan13Features vulkan13_features = {
        .sType = sType(vulkan13_features),
        .synchronization2 = true,
        .dynamicRendering = true,
    };
    
    VkDeviceCreateInfo create_info = {
        .sType = sType(create_info),
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
}

Context CreateContext(const ContextConfig& config, Device dev) {
    auto ctx = std::make_unique<ContextImpl>(ContextImpl{
        .device = CreateDevice(config, dev),
    });
    if (!ctx->device) {
        throw std::runtime_error{"Vulkan: Failed to create context"};
    }
    return ctx.release();
}

void DestroyContext(Context ctx) {
    delete ctx;
}
}
