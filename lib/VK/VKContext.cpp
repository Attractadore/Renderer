#include "VKContext.hpp"
#include "VKSwapchain.hpp"

#include <ranges>

namespace R1::VK {
namespace {
VkDevice createDevice(
    VkInstance instance,
    VkPhysicalDevice physical_device, const QueueFamilies& queue_families,
    std::span<const char* const> extensions
) {
    VkDevice dev = VK_NULL_HANDLE;

    float priority = 1.0f;
    VkDeviceQueueCreateInfo queue_create_info = {
        .sType = sType(queue_create_info),
        .queueFamilyIndex =
            static_cast<uint32_t>(queue_families.graphics),
        .queueCount = 1,
        .pQueuePriorities = &priority,
    };

    VkPhysicalDeviceVulkan13Features vulkan13_features = {
        .sType = sType(vulkan13_features),
        .synchronization2 = true,
        .dynamicRendering = true,
    };
    
    VkDeviceCreateInfo create_info = {
        .sType = sType(create_info),
        .pNext = &vulkan13_features,
        .queueCreateInfoCount = 1,
        .pQueueCreateInfos = &queue_create_info,
        .enabledExtensionCount =
            static_cast<uint32_t>(extensions.size()),
        .ppEnabledExtensionNames = extensions.data(),
    };

    vkCreateDevice(physical_device, &create_info, nullptr, &dev);
    return dev;
}

std::vector<PresentMode> getPresentModes(
    VkPhysicalDevice dev, VkSurfaceKHR surf
) {
    std::vector<VkPresentModeKHR> pmodes(getSurfacePresentModeCount(dev, surf));
    getSurfacePresentModes(dev, surf, pmodes.begin());
    auto v = std::views::transform(pmodes, PresentModeFromVK);
    return std::vector<PresentMode>(v.begin(), v.end());
}
}

Context::Context(
    VkInstance instance,
    VkPhysicalDevice physical_device,
    const QueueFamilies& queue_families,
    VkSurfaceKHR surf
):
    m_physical_device{physical_device},
    m_queue_families{queue_families},
    m_device {createDevice(
        instance, m_physical_device,
        queue_families,
        required_device_extensions
    )},
    m_queues {
        .graphics = [&] {
            VkQueue q;
            vkGetDeviceQueue(m_device.get(), m_queue_families.graphics, 0, &q);
            return q;
        } (),
    },
    m_surface{surf},
    m_present_modes{getPresentModes(m_physical_device, m_surface)}
{
    if (!m_device) {
        throw std::runtime_error{"Vulkan: Failed to create context"};
    }
}

Context::~Context() = default;

size_t Context::presentModeCount() const {
    return m_present_modes.size();
}

size_t Context::presentModes(std::span<PresentMode> out) const {
    auto cnt = std::min(out.size(), presentModeCount());
    std::copy_n(m_present_modes.begin(), cnt, out.begin());
    return cnt;
}

Swapchain* Context::createSwapchain(SizeCallback&& size_cb, PresentMode pmode) {
    assert(!m_swapchain);
    auto vk_pmode = PresentModeToVK(pmode);
    m_swapchain = std::make_unique<Swapchain>(
        m_physical_device, m_device.get(), m_surface,
        std::move(size_cb), VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, vk_pmode
    );
    init_draw();
    return m_swapchain.get();
}

GraphicsPipelineBuilder Context::createGraphicsPipelineBuilder() {
    return GraphicsPipelineBuilder(m_device.get());
}
}

#include "VKContextDraw.cpp"
