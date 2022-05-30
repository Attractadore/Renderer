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

PipelineLayoutRef Context::CreatePipelineLayout(const PipelineLayoutConfig& config) {
    VkPipelineLayout layout = VK_NULL_HANDLE;
    VkPipelineLayoutCreateInfo create_info = {
        .sType = sType(create_info),
    };
    vkCreatePipelineLayout(m_device.get(), &create_info, nullptr, &layout);
    if (!layout) {
        throw std::runtime_error{"Vulkan: Failed to create pipeline layout\n"};
    }
    return PipelineLayout::Create(Vk::PipelineLayout{m_device.get(), layout});
}

ShaderModuleRef Context::CreateShaderModule(const ShaderModuleConfig& config) {
    const auto& code = config.code;
    VkShaderModule module = VK_NULL_HANDLE;
    VkShaderModuleCreateInfo create_info = {
        .sType = sType(create_info),
        .codeSize = code.size(),
        .pCode = reinterpret_cast<const uint32_t*>(code.data()),
    };
    vkCreateShaderModule(m_device.get(), &create_info, nullptr, &module);
    if (!module) {
        throw std::runtime_error{"Vulkan: Failed to create shader module\n"};
    }
    return ShaderModule::Create(Vk::ShaderModule{m_device.get(), module});
}

FenceRef Context::CreateFence(const Fence::Config& config) {
    VkFence fence = VK_NULL_HANDLE;
    VkFenceCreateInfo create_info = {
        .sType = sType(create_info),
        .flags = VkFenceCreateFlags(
            config.signaled ? VK_FENCE_CREATE_SIGNALED_BIT : 0
        ),
    };
    vkCreateFence(m_device.get(), &create_info, nullptr, &fence);
    if (!fence) {
        throw std::runtime_error{"Vulkan: Failed to create fence"};
    }
    return Fence::Create(Vk::Fence{m_device.get(), fence});
}

Fence::Status Context::WaitForFence(Fence& fence, std::chrono::nanoseconds timeout) {
    VkFence h = fence.m_fence.get();
    auto r = vkWaitForFences(m_device.get(), 1, &h, true, timeout.count());
    if (r == VK_SUCCESS) {
        return Fence::Status::Ready;
    } else if (r == VK_TIMEOUT) {
        return Fence::Status::NotReady;
    } else {
        throw std::runtime_error{"Vulkan: Failed to wait for fence"};
    }
}

void Context::ResetFence(Fence& fence) {
    VkFence h = fence.m_fence.get();
    if (vkResetFences(m_device.get(), 1, &h)) {
        throw std::runtime_error{"Vulkan: Failed to reset fence"};
    }
}

SemaphoreRef Context::CreateSemaphore(const Semaphore::Config& config) {
    VkSemaphore sem = VK_NULL_HANDLE;
    VkSemaphoreCreateInfo create_info = {
        .sType = sType(create_info),
    };
    vkCreateSemaphore(m_device.get(), &create_info, nullptr, &sem);
    if (!sem) {
        throw std::runtime_error{"Vulkan: Failed to create semaphore"};
    }
    return Semaphore::Create(Vk::Semaphore{m_device.get(), sem});
}
}

#include "VKContextDraw.cpp"
