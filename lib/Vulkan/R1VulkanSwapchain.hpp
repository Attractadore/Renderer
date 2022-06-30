#pragma once
#include "R1Types.h"
#include "R1VulkanContext.hpp"
#include "GAPI/Vulkan/GALRAII.hpp"
#include "Swapchain.hpp"

#include <unordered_map>

namespace R1 {
struct VulkanSwapchainConfig {
    VkPresentModeKHR present_mode;
};

struct VulkanSwapchainPresentImageConfig {
    GAL::Format         format;
    GAL::ImageLayout    start_layout;
    GAL::ImageLayout    end_layout;
    unsigned            width, height;
};

namespace Detail {
using CommandBufferSetBase = std::vector<GAL::CommandBuffer>;

class CommandBufferSet: private CommandBufferSetBase {
    GAL::Context        m_ctx;
    GAL::CommandPool    m_pool;

public:
    CommandBufferSet(
        GAL::Context ctx = nullptr,
        GAL::CommandPool pool = nullptr,
        size_t count = 0
    ):
        CommandBufferSetBase{count},
        m_ctx{ctx},
        m_pool{pool}
    {
        if (m_ctx) {
            assert(m_pool);
            GAL::AllocateCommandBuffers(m_ctx, m_pool, *this);
        }
    }
    CommandBufferSet(const CommandBufferSet&) = delete;
    CommandBufferSet(CommandBufferSet&&) = default;
    CommandBufferSet& operator=(const CommandBufferSet&) = delete;
    CommandBufferSet& operator=(CommandBufferSet&&) = default;
    ~CommandBufferSet() {
        if (m_ctx) {
            assert(m_pool);
            GAL::FreeCommandBuffers(m_ctx, m_pool, *this);
        }
    }

    using CommandBufferSetBase::begin;
    using CommandBufferSetBase::end;
    using CommandBufferSetBase::operator[];
};
}

class VulkanSwapchain: public Swapchain {
    GAPI::Context&              m_ctx;
    GAPI::Vulkan::HSwapchain    m_swapchain;
    struct Syncs {
        GAPI::HSemaphore        acquire_semaphore;
        GAPI::Vulkan::HFence    acquire_fence;
        GAPI::HSemaphore        present_semaphore;
        GAPI::Vulkan::HFence    present_fence;
    };
    std::vector<Syncs>          m_syncs;
    unsigned                    m_acquire_idx = 0;
    unsigned                    m_current_image_idx;

    GAPI::HCommandPool          m_cmd_pool;
    Detail::CommandBufferSet    m_cmd_buffers;
    struct TransferConfig {
        GAL::Image image;
        GAL::Image swc_image;

        bool operator==(const TransferConfig&) const noexcept = default;
    };

    struct TransferConfigHasher {
        size_t operator()(const TransferConfig& tcfg) const noexcept {
            std::hash<GAL::Image> h;
            return (h(tcfg.image) << 5) ^ h(tcfg.swc_image);
        }
    };

    std::unordered_map<
        TransferConfig,
        GAL::CommandBuffer,
        TransferConfigHasher>   m_cmd_buffer_map;

public:
    VulkanSwapchain(
        R1::VulkanContext& ctx,
        VkSurfaceKHR surface,
        GAL::Vulkan::SurfaceSizeCallback size_cb,
        const VulkanSwapchainConfig& config
    );

    std::tuple<unsigned, unsigned> Size() const {
        return GAL::Vulkan::GetSwapchainSize(m_swapchain.get());
    }

    void AcquireImage();
    void ConfigPresentForImages(
        std::span<const GAL::Image> images,
        const VulkanSwapchainPresentImageConfig& config
    );
    void PresentImage(
        GAL::Image image,
        GAL::Semaphore semaphore,
        GAL::SemaphorePayload wait_value,
        GAL::SemaphorePayload signal_value
    );

private:
    GAL::Queue GetPresentQueue() const noexcept {
        return m_ctx.GetGraphicsQueue();
    }

    size_t GetImageCount() const noexcept {
        return GAL::Vulkan::GetSwapchainImageCount(m_swapchain.get());
    }

    GAL::Image GetImage(size_t idx) const noexcept {
        return GAL::Vulkan::GetSwapchainImage(m_swapchain.get(), idx);
    }

    void CreateSyncs();
    void Resize();

    void SubmitTransferCommands(
        GAL::Image image,
        GAL::Semaphore image_semaphore,
        GAL::SemaphorePayload wait_value,
        GAL::SemaphorePayload signal_value
    );

    GAL::Semaphore GetCurrentAcquireSemaphore() const noexcept {
        return m_syncs[m_acquire_idx].acquire_semaphore.get();
    }

    GAL::Vulkan::Fence GetCurrentAcquireFence() const noexcept {
        return m_syncs[m_acquire_idx].acquire_fence.get();
    }

    GAL::Semaphore GetCurrentPresentSemaphore() const noexcept {
        return m_syncs[m_current_image_idx].present_semaphore.get();
    }

    GAL::Vulkan::Fence GetCurrentPresentFence() const noexcept {
        return m_syncs[m_current_image_idx].present_fence.get();
    }

    GAL::Image GetCurrentImage() const noexcept {
        return GetImage(m_current_image_idx);
    }

    GAL::CommandBuffer GetCommandBuffer(
        GAL::Image image, GAL::Image swc_image
    ) const noexcept;
};

inline GAL::Vulkan::SurfaceSizeCallback MakeSurfaceSizeCallback(
    R1SurfaceSizeCallback cb, void* usrptr
) {
    return [=] {
        int w, h;
        cb(usrptr, &w, &h);
        if (w < 0 or h < 0) {
            throw std::runtime_error{
                "Vulkan: Failed to get surface size"};
        }
        return std::tuple<unsigned, unsigned>{w, h};
    };
}
}
