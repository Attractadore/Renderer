#include "R1Impl.hpp"
#include "R1Vulkan.h"
#include "R1VulkanImpl.hpp"

extern "C" {
R1Instance* R1_VK_CreateInstanceFromTemplate(
    PFN_vkGetInstanceProcAddr loader,
    const VkInstanceCreateInfo* create_template
) {
    return new R1::VulkanInstance{loader, create_template};
}

void R1_DestroyInstance(R1Instance* instance) {
    delete R1::ToPrivate(instance);
}

void R1_VK_GetInstanceInfo(R1Instance* instance, R1VKInstanceInfo* info_out) {
    auto vk_instance = R1::ToPrivate(instance)->GetVkInstance();
    *info_out = {
        .instance = vk_instance,
    };
}

void R1_VK_GetDeviceInfo(R1Device* device, R1VKDeviceInfo* info) {
    auto physical_device =
        R1::GAL::Vulkan::GetVkPhysicalDevice(R1::ToPrivate(device)->get());
    *info = {
        .physical_device = physical_device,
    };
}

R1Context* R1_VK_CreateContextFromTemplate(
    R1Device* device,
    const VkDeviceCreateInfo* create_template
) {
    return new R1::VulkanContext{*R1::ToPrivate(device), create_template};
}

void R1_DestroyContext(R1Context* ctx) {
    delete R1::ToPrivate(ctx); 
}

void R1_VK_GetContextInfo(R1Context* ctx, R1VKContextInfo* info) {
    *info = {
        .device = R1::ToPrivate(ctx)->GetVkDevice(),
    };
}

R1Swapchain* R1_VK_CreateSwapchain(
    R1Context* ctx,
    const R1VKSwapchainCreateInfo* create_info
) {
    return new R1::VulkanSwapchain{
        *R1::ToPrivate(ctx),
        create_info->surface,
        R1::MakeSurfaceSizeCallback(create_info->surface_size_callback, create_info->usrptr),
        {
            .present_mode = create_info->present_mode,
        }};
}

void R1_DestroySwapchain(R1Swapchain* swapchain) {
    delete R1::ToPrivate(swapchain);
}

R1Scene* R1_CreateScene(R1Context* ctx) {
    return new R1::VulkanScene{*ctx};
}

void R1_DestroyScene(R1Scene* scene) {
    delete R1::ToPrivate(scene);
}

void R1_VK_ConfigSceneOutputImages(R1Scene* scene,
    unsigned            width,
    unsigned            height,
    unsigned            count,
    VkImageUsageFlags   image_usage_flags
) {
    scene->ConfigOutputImages(
        width, height, count,
        static_cast<R1::GAL::ImageUsage>(image_usage_flags));
}

void R1_VK_GetSceneOutputInfo(R1Scene* scene, R1VKSceneOutputInfo* info) {
    auto impl = R1::ToPrivate(scene);
    std::tie(info->width, info->height) = impl->GetOutputImageSize();
    info->count = impl->GetOutputImageCount();
    info->format = static_cast<VkFormat>(impl->GetOutputImageFormat());
    info->start_layout =
        static_cast<VkImageLayout>(impl->GetOutputImageStartLayout());
    info->end_layout =
        static_cast<VkImageLayout>(impl->GetOutputImageEndLayout());
}

void R1_VK_GetSceneOutputImageInfo(
    R1Scene* scene, unsigned idx, R1VKSceneOutputImageInfo* info
) {
    auto impl = R1::ToPrivate(scene);
    *info = {
        .image = R1::GAL::Vulkan::GetVkImage(impl->GetOutputImage(idx)),
    };
}

void R1_VK_DrawScene(R1Scene* scene, R1VKPresentInfo* present_info) {
    auto impl = R1::ToPrivate(scene);
    auto idx = impl->GetCurrentOutputImage();
    auto pres_info = impl->Draw();
    *present_info = {
        .ready_semaphore = pres_info.semaphore,
        .wait_value = pres_info.wait_value,
        .signal_value = pres_info.signal_value,
        .image_idx = idx,
    };
}

void R1_DrawSceneToSwapchain(R1Scene* scene, R1Swapchain* swapchain) {
    auto swc_impl = R1::ToPrivate(swapchain);

    swc_impl->AcquireImage();

    auto [swc_w, swc_h] = swc_impl->Size();
    if (std::tie(swc_w, swc_h) != scene->GetOutputImageSize()) {
        constexpr auto count = 2;
        scene->ConfigOutputImages(
            swc_w, swc_h, count, R1::GAL::ImageUsage::TransferSRC);

        std::vector<R1::GAL::Image> images(count);
        for (size_t i = 0; i < count; i++) {
            images[i] = scene->GetOutputImage(i);
        }

        R1::VulkanSwapchainPresentImageConfig config = {
            .format = scene->GetOutputImageFormat(),
            .start_layout = scene->GetOutputImageStartLayout(),
            .end_layout = scene->GetOutputImageEndLayout(),
            .width = swc_w,
            .height = swc_h,
        };
        swc_impl->ConfigPresentForImages(images, config);
    }

    auto scene_img_idx = scene->GetCurrentOutputImage();
    auto pres_info = scene->Draw();

    swc_impl->PresentImage(
        scene->GetOutputImage(scene_img_idx),
        pres_info.semaphore,
        pres_info.wait_value,
        pres_info.signal_value);
}
}
