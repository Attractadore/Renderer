#pragma once
#include "R1Types.h"

#include <vulkan/vulkan.h>

#ifdef __cplusplus
extern "C" {
#endif

R1Instance* R1_VK_CreateInstanceFromTemplate(
    PFN_vkGetInstanceProcAddr get_instance_proc_addr,
    const VkInstanceCreateInfo* create_info_template
);

typedef struct {
    VkInstance instance;
} R1VKInstanceInfo;

void R1_VK_GetInstanceInfo(R1Instance* instance, R1VKInstanceInfo* info);

typedef struct {
    VkPhysicalDevice physical_device;
} R1VKDeviceInfo;

void R1_VK_GetDeviceInfo(R1Device* device, R1VKDeviceInfo* info);

R1Context* R1_VK_CreateContextFromTemplate(
    R1Device* device,
    const VkDeviceCreateInfo* create_info_template
);

typedef struct {
    VkDevice device;
} R1VKContextInfo;

void R1_VK_GetContextInfo(R1Context* ctx, R1VKContextInfo* info);

typedef struct {
    VkSurfaceKHR            surface;
    R1SurfaceSizeCallback   surface_size_callback;
    void*                   usrptr;
    VkPresentModeKHR        present_mode;
} R1VKSwapchainCreateInfo;

R1Swapchain* R1_VK_CreateSwapchain(
    R1Context* ctx, const R1VKSwapchainCreateInfo* create_info
);

void R1_VK_ConfigSceneOutputImages(R1Scene* scene,
    unsigned            width,
    unsigned            height,
    unsigned            count,
    VkImageUsageFlags   image_usage_flags
);

typedef struct {
    unsigned        width, height;
    unsigned        count;
    VkFormat        format;
    VkImageLayout   start_layout;
    VkImageLayout   end_layout;
} R1VKSceneOutputInfo;

void R1_VK_GetSceneOutputInfo(
    R1Scene* scene, R1VKSceneOutputInfo* info
);

typedef struct {
    VkImage         image;
} R1VKSceneOutputImageInfo;

void R1_VK_GetSceneOutputImageInfo(
    R1Scene* scene, unsigned idx, R1VKSceneOutputImageInfo* info
);

typedef struct {
    VkSemaphore ready_semaphore;
    uint64_t    wait_value;
    uint64_t    signal_value;
    size_t      image_idx;
} R1VKPresentInfo;

void R1_VK_DrawScene(R1Scene* scene, R1VKPresentInfo* present_info);

#ifdef __cplusplus
}
#endif
