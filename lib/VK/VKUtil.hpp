#pragma once
#include "Common/Util.hpp"

#include <concepts>

#include <vulkan/vulkan.h>
#include <X11/Xlib.h>
#include <xcb/xcb.h>
#include <vulkan/vulkan_xlib.h>
#include <vulkan/vulkan_xcb.h>

namespace R1::VK {
template<typename S>
consteval VkStructureType sType() {
    using T = std::remove_cvref_t<S>;
    if constexpr (std::same_as<T, VkApplicationInfo>) {
        return VK_STRUCTURE_TYPE_APPLICATION_INFO; 
    }
    if constexpr (std::same_as<T, VkInstanceCreateInfo>) {
        return VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    }
    if constexpr (std::same_as<T, VkXlibSurfaceCreateInfoKHR>) {
        return VK_STRUCTURE_TYPE_XLIB_SURFACE_CREATE_INFO_KHR;
    }
    if constexpr (std::same_as<T, VkXcbSurfaceCreateInfoKHR>) {
        return VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR;
    }
    if constexpr (std::same_as<T, VkPhysicalDeviceDynamicRenderingFeatures>) {
        return VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES;
    }
    if constexpr (std::same_as<T, VkPhysicalDeviceVulkan13Features>) {
        return VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES;
    }
    if constexpr (std::same_as<T, VkDeviceQueueCreateInfo>) {
        return VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    }
    if constexpr (std::same_as<T, VkDeviceCreateInfo>) {
        return VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    }
    if constexpr (std::same_as<T, VkSwapchainCreateInfoKHR>) {
        return VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    }
    if constexpr (std::same_as<T, VkPresentInfoKHR>) {
        return VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    }
    if constexpr (std::same_as<T, VkPipelineLayoutCreateInfo>) {
        return VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    }
    if constexpr (std::same_as<T, VkPipelineRenderingCreateInfo>) {
        return VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;
    }
    if constexpr (std::same_as<T, VkShaderModuleCreateInfo>) {
        return VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    }
    if constexpr (std::same_as<T, VkPipelineShaderStageCreateInfo>) {
        return VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    }
    if constexpr (std::same_as<T, VkPipelineVertexInputStateCreateInfo>) {
        return VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    }
    if constexpr (std::same_as<T, VkPipelineVertexInputStateCreateInfo>) {
        return VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    }
    if constexpr (std::same_as<T, VkPipelineInputAssemblyStateCreateInfo>) {
        return VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    }
    if constexpr (std::same_as<T, VkPipelineTessellationStateCreateInfo>) {
        return VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO;
    }
    if constexpr (std::same_as<T, VkPipelineViewportStateCreateInfo>) {
        return VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    }
    if constexpr (std::same_as<T, VkPipelineRasterizationStateCreateInfo>) {
        return VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    }
    if constexpr (std::same_as<T, VkPipelineMultisampleStateCreateInfo>) {
        return VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    }
    if constexpr (std::same_as<T, VkPipelineDepthStencilStateCreateInfo>) {
        return VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    }
    if constexpr (std::same_as<T, VkPipelineColorBlendStateCreateInfo>) {
        return VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    }
    if constexpr (std::same_as<T, VkPipelineDynamicStateCreateInfo>) {
        return VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    }
    if constexpr (std::same_as<T, VkGraphicsPipelineCreateInfo>) {
        return VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    }
    if constexpr (std::same_as<T, VkImageViewCreateInfo>) {
        return VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    }
    if constexpr (std::same_as<T, VkCommandPoolCreateInfo>) {
        return VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    }
    if constexpr (std::same_as<T, VkCommandBufferAllocateInfo>) {
        return VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    }
    if constexpr (std::same_as<T, VkFenceCreateInfo>) {
        return VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    }
    if constexpr (std::same_as<T, VkSemaphoreCreateInfo>) {
        return VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    }
    if constexpr (std::same_as<T, VkImageMemoryBarrier2>) {
        return VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
    }
    if constexpr (std::same_as<T, VkDependencyInfo>) {
        return VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
    }
    if constexpr (std::same_as<T, VkCommandBufferSubmitInfo>) {
        return VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO;
    }
    if constexpr (std::same_as<T, VkSemaphoreSubmitInfo>) {
        return VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
    }
    if constexpr (std::same_as<T, VkSubmitInfo2>) {
        return VK_STRUCTURE_TYPE_SUBMIT_INFO_2;
    }
    throw "Unknown sType";
}

template<typename T>
consteval VkStructureType sType(const T&) {
    return sType<T>();
}
}
