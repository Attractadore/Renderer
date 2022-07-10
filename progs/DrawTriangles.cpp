#include "R1/R1.h"
#include "R1/R1Vulkan.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_vulkan.h>

#include <glm/gtc/type_ptr.hpp>
#include <glm/mat4x4.hpp>

#include <array>
#include <iostream>
#include <vector>

namespace {
static constexpr auto app_name = "Draw triangles";

PFN_vkGetInstanceProcAddr GetVkGetInstanceProcAddr() {
    return reinterpret_cast<PFN_vkGetInstanceProcAddr>(
        SDL_Vulkan_GetVkGetInstanceProcAddr());
}

R1Instance* CreateInstance() {
    unsigned ext_cnt = 0;
    if (!SDL_Vulkan_GetInstanceExtensions(nullptr, &ext_cnt, nullptr)) {
        return nullptr;
    }
    std::vector<const char*> extensions(ext_cnt);
    if (!SDL_Vulkan_GetInstanceExtensions(nullptr, &ext_cnt, extensions.data())) {
        return nullptr;
    }
    extensions.resize(ext_cnt);

    VkApplicationInfo application_info = {
        .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .pApplicationName = app_name,
    };

    VkInstanceCreateInfo create_template = {
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pApplicationInfo = &application_info,
        .enabledExtensionCount =
            static_cast<uint32_t>(extensions.size()),
        .ppEnabledExtensionNames = extensions.data(),
    };
    return R1_VK_CreateInstanceFromTemplate(
        GetVkGetInstanceProcAddr(),
        &create_template);
}

R1Context* CreateContext(R1Device* device) {
    constexpr auto swc_ext = VK_KHR_SWAPCHAIN_EXTENSION_NAME;
    VkDeviceCreateInfo create_template = {
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .enabledExtensionCount = 1,
        .ppEnabledExtensionNames = &swc_ext,
    };
    return R1_VK_CreateContextFromTemplate(device, &create_template);
}

VkSurfaceKHR CreateSurface(R1Instance* instance, SDL_Window* window) {
    R1VKInstanceInfo info;
    R1_VK_GetInstanceInfo(instance, &info);
    VkSurfaceKHR surface = VK_NULL_HANDLE;
    SDL_Vulkan_CreateSurface(window, info.instance, &surface);
    return surface;
}

void DestroySurface(R1Instance* instance, VkSurfaceKHR surf) {
    auto vk_get_instance_proc_addr = GetVkGetInstanceProcAddr();
    if (!vk_get_instance_proc_addr) { return; }
    R1VKInstanceInfo info;
    R1_VK_GetInstanceInfo(instance, &info);
    auto vk_destroy_surface = reinterpret_cast<PFN_vkDestroySurfaceKHR>(
        vk_get_instance_proc_addr(info.instance, "vkDestroySurfaceKHR"));
    if (!vk_destroy_surface) { return; }
    vk_destroy_surface(info.instance, surf, nullptr);
}
}

int main() {
    SDL_Init(SDL_INIT_EVERYTHING);
    auto window = SDL_CreateWindow(
        app_name,
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        640, 480,
        SDL_WINDOW_RESIZABLE | SDL_WINDOW_VULKAN
    );

    auto instance = CreateInstance();
    if (!instance) {
        std::cerr << "Failed to create renderer instance\n";
        return -1;
    }
    if (!R1_GetDeviceCount(instance)) {
        std::cerr << "Failed to create renderer context: no devices\n";
        return -1;
    }
    auto dev = R1_GetDevice(instance, 0);
    std::cout << "Running on " << R1_GetDeviceName(dev) << "\n";
    auto ctx = CreateContext(dev);
    if (!ctx) {
        std::cerr << "Failed to create renderer context\n";
        return -1;
    }
    auto surf = CreateSurface(instance, window);
    if (!surf) {
        std::cerr << "Failed to create window surface\n";
        return -1;
    }
    auto swc = [&] {
        auto surface_size_callback = [] (void* window, int* w, int* h) {
            SDL_GetWindowSize(reinterpret_cast<SDL_Window*>(window), w, h);
        };
        R1VKSwapchainCreateInfo create_info = {
            .surface = surf,
            .surface_size_callback = surface_size_callback,
            .usrptr = window,
            .present_mode = VK_PRESENT_MODE_FIFO_KHR,
        };
        return R1_VK_CreateSwapchain(ctx, &create_info);
    } ();
    if (!swc) {
        std::cerr << "Failed to create swapchain\n";
        return -1;
    }
    auto scene = R1_CreateScene(ctx);
    if (!scene) {
        std::cerr << "Failed to create scene\n";
        return -1;
    }

    std::array<glm::vec3, 3> data = {{
        { 0.0f,  0.5f, 0.0f},
        { 0.5f, -0.5f, 0.0f},
        {-0.5f, -0.5f, 0.0f},
    }};
    R1MeshConfig mesh_config = {
        .vertices = glm::value_ptr(data[0]),
        .vertex_count = 3,
    };
    auto tri_mesh = R1_CreateMesh(scene, &mesh_config);

    std::array<glm::mat4, 4> tri_translations = {{
        glm::translate(glm::mat4{1.0f}, { 0.5f,  0.5f, 0.0f}),
        glm::translate(glm::mat4{1.0f}, {-0.5f,  0.5f, 0.0f}),
        glm::translate(glm::mat4{1.0f}, { 0.5f, -0.5f, 0.0f}),
        glm::translate(glm::mat4{1.0f}, {-0.5f, -0.5f, 0.0f}),
    }};
    std::array<R1MeshInstance, 4> tri_models;
    for (size_t i = 0; i < tri_models.size(); i++) {
        auto transform =
            tri_translations[i] *
            glm::scale(glm::mat4{1.0f}, {0.5f, 0.5f, 0.5f});
        R1MeshInstanceConfig model_config = {
            .mesh = tri_mesh,
        };
        std::memcpy(model_config.transform, &transform, sizeof(transform));
        tri_models[i] = R1_CreateMeshInstance(scene, &model_config);
    }

    bool quit = false;
    while (!quit) {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                quit = true;
            }
        }

        R1_DrawSceneToSwapchain(scene, swc);
    }

    for (auto tri_model: tri_models) {
        R1_DestroyMeshInstance(scene, tri_model);
    }
    R1_DestroyMesh(scene, tri_mesh);
    R1_DestroyScene(scene);
    R1_DestroySwapchain(swc);
    DestroySurface(instance, surf);
    R1_DestroyContext(ctx);
    R1_DestroyInstance(instance);

    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
