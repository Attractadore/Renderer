#pragma once
#include "VKPipeline.hpp"
#include "VKSync.hpp"
#include "VKTypes.hpp"

#include <algorithm>
#include <span>

namespace R1::VK {
constexpr std::array required_device_extensions = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME,
};

class Context {
    VkPhysicalDevice            m_physical_device;
    QueueFamilies               m_queue_families;
    Vk::Device                  m_device;
    Queues                      m_queues;
    VkSurfaceKHR                m_surface;
    std::unique_ptr<Swapchain>  m_swapchain;
    std::vector<PresentMode>    m_present_modes;

    struct DrawData;
    std::unique_ptr<DrawData>   m_draw_data;

public:
    Context(
        VkInstance instance,
        VkPhysicalDevice physical_device,
        const QueueFamilies& queue_families,
        VkSurfaceKHR surf
    );
    ~Context();

    size_t presentModeCount() const;
    size_t presentModes(std::span<PresentMode> out) const;

    Swapchain* createSwapchain(SizeCallback&& size_cb, PresentMode pmode);

    PipelineLayoutRef CreatePipelineLayout(const PipelineLayoutConfig& config);
    ShaderModuleRef CreateShaderModule(const ShaderModuleConfig& config);

    template<std::output_iterator<PipelineRef> Iter>
    Iter CreateGraphicsPipelines(
        const GraphicsPipelineConfigs& pipeline_configs, Iter out
    );

    FenceRef CreateFence(const Fence::Config& config);
    Fence::Status WaitForFence(Fence& fence, std::chrono::nanoseconds timeout = Fence::WaitNoTimeout);
    template<typename Rep, typename Period>
    Fence::Status WaitForFence(Fence& fence, std::chrono::duration<Rep, Period> timeout) {
        return WaitForFence(fence, std::chrono::duration_cast<std::chrono::nanoseconds>(timeout));
    }
    void ResetFence(Fence& fence);
    SemaphoreRef CreateSemaphore(const Semaphore::Config& config);

    void init_draw();
    void draw();
};

template<std::output_iterator<PipelineRef> Iter>
Iter Context::CreateGraphicsPipelines(
    const GraphicsPipelineConfigs& pipeline_configs, Iter out
) {
    auto cnt = pipeline_configs.create_infos.size();
    std::vector<VkPipeline> pipelines(cnt);
    std::vector<Vk::Pipeline> handles;
    handles.reserve(cnt);
    auto r = vkCreateGraphicsPipelines(
        m_device.get(),
        VK_NULL_HANDLE,
        cnt,
        pipeline_configs.create_infos.data(),
        nullptr,
        reinterpret_cast<VkPipeline*>(pipelines.data())
    );
    if (r != VK_SUCCESS) {
        throw std::runtime_error{"Vulkan: Failed to create pipelines"};
    }
    std::ranges::transform(
        pipelines, std::back_inserter(handles),
        [&] (VkPipeline pipeline) {
            return Vk::Pipeline{m_device.get(), pipeline};
        }
    );
    return std::ranges::transform(
        handles, out,
        [&] (Vk::Pipeline& handle) {
            return Pipeline::Create(std::move(handle));
        }
    ).out;
}
}
