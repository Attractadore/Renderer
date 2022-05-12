#pragma once
#include "Common/Interfaces.hpp"
#include "VKRAII.hpp"
#include "VKTypes.hpp"

#undef None
#undef Always
#include "VKPipelineBuilder.hpp"

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

    GraphicsPipelineBuilder createGraphicsPipelineBuilder();

    void init_draw();
    void draw();
};
static_assert(IContext<Context>);
}
