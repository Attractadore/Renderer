#include "VKContext.hpp"
#include "VKSwapchain.hpp"

#include <fstream>
#include <filesystem>
#include <ranges>

namespace R1::VK {
namespace {
std::vector<std::byte> loadShader(const std::string& path) {
    std::ifstream f{path, std::ios_base::binary};
    auto sz = std::filesystem::file_size(path);
    std::vector<std::byte> data(sz);
    f.read(reinterpret_cast<char*>(data.data()), sz);
    return data;
}

VkPipelineLayout createPipelineLayout(VkDevice dev) {
    VkPipelineLayoutCreateInfo create_info = {
        .sType = sType(create_info),
    };
    VkPipelineLayout layout;
    vkCreatePipelineLayout(dev, &create_info, nullptr, &layout);
    return layout;
}

VkPipeline createPipeline(
    Context& ctx,
    VkDevice dev, VkFormat color_fmt,
    std::span<std::byte> vert_code,
    std::span<std::byte> frag_code,
    VkPipelineLayout layout
) {
    VkShaderModule vert_shader_module, frag_shader_module;
    {
        VkShaderModuleCreateInfo vert_create_info = {
            .sType = sType(vert_create_info),
            .codeSize = vert_code.size(),
            .pCode = reinterpret_cast<const uint32_t*>(vert_code.data()),
        };

        VkShaderModuleCreateInfo frag_create_info = {
            .sType = sType(frag_create_info),
            .codeSize = frag_code.size(),
            .pCode = reinterpret_cast<const uint32_t*>(frag_code.data()),
        };

        vkCreateShaderModule(dev, &vert_create_info, nullptr, &vert_shader_module);
        vkCreateShaderModule(dev, &frag_create_info, nullptr, &frag_shader_module);
    }

    auto gpb = ctx.createGraphicsPipelineBuilder();
    gpb.setLayout({.layout = layout}).
        setVertexStage({.module = vert_shader_module}).
        setFragmentStage({.module = frag_shader_module}).
        setVertexInputState({}).
        setInputAssemblyState({}).
        setRasterizationState({}).
        setMultisampleState({}).
        setColorState({ .attachments {
            { .format = static_cast<ColorFormat>(color_fmt) }
        }});
    auto pipeline = gpb.build().pipeline;

    vkDestroyShaderModule(dev, vert_shader_module, nullptr);
    vkDestroyShaderModule(dev, frag_shader_module, nullptr);

    return pipeline;
}

VkImageView createSwapchainImageView(
    VkDevice dev, VkImage img, VkFormat fmt 
) {
    VkImageViewCreateInfo create_info = {
        .sType = sType(create_info),
        .image = img,
        .viewType = VK_IMAGE_VIEW_TYPE_2D,
        .format = fmt,
        .subresourceRange = {
            .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
            .baseMipLevel = 0,
            .levelCount = 1,
            .baseArrayLayer = 0,
            .layerCount = 1,
        },
    };
    VkImageView view;
    vkCreateImageView(dev, &create_info, nullptr, &view);
    return view;
}

VkCommandPool createCommandPool(VkDevice dev, unsigned graphics_queue_family) {
    VkCommandPoolCreateInfo create_info = {
        .sType = sType(create_info),
        .flags =
            VK_COMMAND_POOL_CREATE_TRANSIENT_BIT |
            VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
        .queueFamilyIndex = graphics_queue_family,
    };
    VkCommandPool command_pool;
    vkCreateCommandPool(dev, &create_info, nullptr, &command_pool);
    return command_pool;
}

std::vector<VkCommandBuffer> createCommandBuffers(
    VkDevice dev, VkCommandPool pool, unsigned n
) {
    VkCommandBufferAllocateInfo alloc_info = {
        .sType = sType(alloc_info),
        .commandPool = pool,
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = n,
    };
    std::vector<VkCommandBuffer> cmd_buffers(alloc_info.commandBufferCount);
    vkAllocateCommandBuffers(dev, &alloc_info, cmd_buffers.data());
    return cmd_buffers;
}

std::vector<VkFence> createFences(VkDevice dev, unsigned n, bool signaled = true) {
    std::vector<VkFence> fences(n);
    for (auto& f: fences) {
        VkFenceCreateInfo create_info = {
            .sType = sType(create_info),
            .flags = VkFenceCreateFlags(
                signaled ? VK_FENCE_CREATE_SIGNALED_BIT : 0
            ),
        };
        vkCreateFence(dev, &create_info, nullptr, &f);
    }
    return fences;
}

std::vector<VkSemaphore> createSemaphores(VkDevice dev, unsigned n) {
    std::vector<VkSemaphore> semaphores(n);
    for (auto& s: semaphores) {
        VkSemaphoreCreateInfo create_info = {
            .sType = sType(create_info),
        };
        vkCreateSemaphore(dev, &create_info, nullptr, &s);
    }
    return semaphores;
}
}

struct Context::DrawData {
    VkDevice                                    device;
    VkFormat                                    image_format;
    std::vector<std::byte>                      vert_code, frag_code;
    VkPipelineLayout                            pipeline_layout;
    VkPipeline                                  pipeline;
    std::unordered_map<VkImage, VkImageView>    image_views;
    unsigned                                    frame_index;
    unsigned                                    frame_count;
    VkCommandPool                               command_pool;
    std::vector<VkCommandBuffer>                command_buffers;
    std::vector<VkFence>                        fences;
    std::vector<VkSemaphore>                    acquire_semaphores;
    std::vector<VkSemaphore>                    draw_semaphores;

    ~DrawData() {
        vkDeviceWaitIdle(device);
        for (auto& f: fences) {
            vkDestroyFence(device, f, nullptr);
        }
        for (auto& s: acquire_semaphores) {
            vkDestroySemaphore(device, s, nullptr);
        }
        for (auto& s: draw_semaphores) {
            vkDestroySemaphore(device, s, nullptr);
        }
        vkFreeCommandBuffers(device, command_pool, command_buffers.size(), command_buffers.data());
        vkDestroyCommandPool(device, command_pool, nullptr);
        for (auto& [_, v]: image_views) {
            vkDestroyImageView(device, v, nullptr);
        }
        vkDestroyPipeline(device, pipeline, nullptr);
        vkDestroyPipelineLayout(device, pipeline_layout, nullptr);
    }
};

void Context::init_draw() {
    m_draw_data = std::make_unique<Context::DrawData>();
    m_draw_data->device = m_device.get();
    m_draw_data->image_format = m_swapchain->description().surface_format.format;
    m_draw_data->vert_code = loadShader("vert.spv");
    m_draw_data->frag_code = loadShader("frag.spv");
    m_draw_data->pipeline_layout = createPipelineLayout(m_device.get());
    m_draw_data->pipeline = createPipeline(
        *this,
        m_device.get(), m_draw_data->image_format,
        m_draw_data->vert_code, m_draw_data->frag_code,
        m_draw_data->pipeline_layout
    );
    m_draw_data->frame_index = 0;
    m_draw_data->frame_count = 2;
    m_draw_data->command_pool = createCommandPool(
        m_device.get(), m_queue_families.graphics
    );
    m_draw_data->command_buffers = createCommandBuffers(
        m_device.get(), m_draw_data->command_pool, m_draw_data->frame_count
    );
    m_draw_data->fences = createFences(
        m_device.get(), m_draw_data->frame_count
    );
    m_draw_data->acquire_semaphores = createSemaphores(
        m_device.get(), m_draw_data->frame_count
    );
    m_draw_data->draw_semaphores = createSemaphores(
        m_device.get(), m_draw_data->frame_count
    );
}

void Context::draw() {
    auto idx = m_draw_data->frame_index;
    VkFence fence = m_draw_data->fences[idx];
    VkSemaphore acquire_sem = m_draw_data->acquire_semaphores[idx];
    VkSemaphore draw_sem = m_draw_data->draw_semaphores[idx];
    VkCommandBuffer cmd_buffer = m_draw_data->command_buffers[idx];
    VkPipeline pipeline = m_draw_data->pipeline;

    vkWaitForFences(m_device.get(), 1, &fence, true, UINT64_MAX);
    vkResetFences(m_device.get(), 1, &fence);

    auto [img, img_w, img_h] = m_swapchain->acquireImage(acquire_sem);
    auto& img_view = m_draw_data->image_views[img];
    if (!img_view) {
        img_view = createSwapchainImageView(
            m_device.get(), img, m_swapchain->description().surface_format.format
        );
    }

    VkCommandBufferBeginInfo begin_info = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
    };
    vkBeginCommandBuffer(cmd_buffer, &begin_info);

    {
        VkImageMemoryBarrier2 from_present = {
            .sType = sType(from_present),
            .srcStageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT,
            .dstStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
            .dstAccessMask = VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT,
            .newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
            .image = img,
            .subresourceRange = {
                .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                .baseMipLevel = 0,
                .levelCount = 1,
                .baseArrayLayer = 0,
                .layerCount = 1,
            },
        };
        VkDependencyInfo dep_info = {
            .sType = sType(dep_info),
            .imageMemoryBarrierCount = 1,
            .pImageMemoryBarriers = &from_present,
        };
        vkCmdPipelineBarrier2(cmd_buffer, &dep_info); 
    }

    VkClearValue clear_color = {0.2f, 0.2f, 0.8f, 1.0f};
    VkRenderingAttachmentInfo color_attachment = {
        .sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
        .imageView = img_view,
        .imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
        .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
        .clearValue = clear_color,
    };

    VkRenderingAttachmentInfo depth_attachment = {
        .sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
    };
    VkRenderingAttachmentInfo stencil_attachment = {
        .sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
    };

    VkRenderingInfo rendering_info = {
        .sType = VK_STRUCTURE_TYPE_RENDERING_INFO,
        .renderArea = { .extent = { img_w, img_h } },
        .layerCount = 1,
        .viewMask = 0,
        .colorAttachmentCount = 1,
        .pColorAttachments = &color_attachment,
        .pDepthAttachment = &depth_attachment,
        .pStencilAttachment = &stencil_attachment,
    };
    vkCmdBeginRendering(cmd_buffer, &rendering_info);

    {
        VkViewport viewport = {
            .width = static_cast<float>(img_w),
            .height = static_cast<float>(img_h),
            .minDepth = 0.0f,
            .maxDepth = 1.0f,
        };
        VkRect2D scissor = {
            .extent = { img_w, img_h }
        };
        vkCmdSetViewportWithCount(cmd_buffer, 1, &viewport);
        vkCmdSetScissorWithCount(cmd_buffer, 1, &scissor);
    }

    VkImageSubresourceRange range = {
        .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
        .baseMipLevel = 0,
        .levelCount = 1,
        .baseArrayLayer = 0,
        .layerCount = 1,
    };

    vkCmdBindPipeline(cmd_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
    vkCmdDraw(cmd_buffer, 3, 1, 0, 0);

    vkCmdEndRendering(cmd_buffer);

    {
        VkImageMemoryBarrier2 to_present = {
            .sType = sType(to_present),
            .srcStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
            .srcAccessMask = VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT,
            .dstStageMask = VK_PIPELINE_STAGE_2_NONE,
            .oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
            .newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
            .image = img,
            .subresourceRange = {
                .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                .baseMipLevel = 0,
                .levelCount = 1,
                .baseArrayLayer = 0,
                .layerCount = 1,
            },
        };
        VkDependencyInfo dep_info = {
            .sType = sType(dep_info),
            .imageMemoryBarrierCount = 1,
            .pImageMemoryBarriers = &to_present,
        };
        vkCmdPipelineBarrier2(cmd_buffer, &dep_info); 
    }

    vkEndCommandBuffer(cmd_buffer);

    {
        VkCommandBufferSubmitInfo cmd_info = {
            .sType = sType(cmd_info),
            .commandBuffer = cmd_buffer,
        };
        VkSemaphoreSubmitInfo wait_info = {
            .sType = sType(wait_info),
            .semaphore = acquire_sem,
            .stageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
        };
        VkSemaphoreSubmitInfo signal_info = {
            .sType = sType(signal_info),
            .semaphore = draw_sem,
            .stageMask = VK_PIPELINE_STAGE_2_NONE,
        };
        VkSubmitInfo2 submit_info = {
            .sType = sType(submit_info),
            .waitSemaphoreInfoCount = 1,
            .pWaitSemaphoreInfos = &wait_info,
            .commandBufferInfoCount = 1,
            .pCommandBufferInfos = &cmd_info,
            .signalSemaphoreInfoCount = 1,
            .pSignalSemaphoreInfos = &signal_info,
        };
        vkQueueSubmit2(m_queues.graphics, 1, &submit_info, fence);
    }

    m_swapchain->present(m_queues.graphics, img, {&draw_sem, 1});

    m_draw_data->frame_index = (m_draw_data->frame_index + 1) % m_draw_data->frame_count;
}
}
