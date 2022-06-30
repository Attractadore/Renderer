#pragma once
#include "GAL/Vulkan/Swapchain.hpp"
#include "ImageImpl.hpp"

namespace R1::GAL::Vulkan {
class SwapchainImpl {
    Context                     ctx;
    VkSwapchainKHR              swapchain;
    std::vector<ImageImpl>      images;
    SurfaceSizeCallback         surface_size_cb;
    VkSwapchainCreateInfoKHR    create_info;

public:
    SwapchainImpl(
        Context ctx,
        VkSurfaceKHR surf,
        SurfaceSizeCallback size_cb,
        const SwapchainConfig& config
    );
    void Init(VkSwapchainKHR old_swapchain = VK_NULL_HANDLE);

    ~SwapchainImpl();

    size_t ImageCount() const noexcept { return images.size(); }
    Image GetImage(unsigned idx) noexcept { return &images[idx]; }
    std::tuple<unsigned, unsigned> Size() const noexcept ;

    std::tuple<unsigned, SwapchainStatus> AcquireImage(
        VkSemaphore signal_semaphore
    );
    SwapchainStatus PresentImage(
        VkQueue queue,
        unsigned image_idx,
        VkSemaphore wait_semaphore
    );
    void Resize();

private:
    void Clear();
};
}
