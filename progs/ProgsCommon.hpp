#pragma once
#include "R1/R1.h"
#include "R1/R1Vulkan.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_vulkan.h>

#include <glm/geometric.hpp>
#include <glm/vec3.hpp>

#include <array>
#include <span>
#include <iostream>
#include <vector>

inline PFN_vkGetInstanceProcAddr GetVkGetInstanceProcAddr() {
    return reinterpret_cast<PFN_vkGetInstanceProcAddr>(
        SDL_Vulkan_GetVkGetInstanceProcAddr());
}

inline R1Instance* CreateInstance(const char* app_name) {
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

inline R1Context* CreateContext(R1Device* device) {
    constexpr auto swc_ext = VK_KHR_SWAPCHAIN_EXTENSION_NAME;
    VkDeviceCreateInfo create_template = {
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .enabledExtensionCount = 1,
        .ppEnabledExtensionNames = &swc_ext,
    };
    return R1_VK_CreateContextFromTemplate(device, &create_template);
}

inline VkSurfaceKHR CreateSurface(R1Instance* instance, SDL_Window* window) {
    R1VKInstanceInfo info;
    R1_VK_GetInstanceInfo(instance, &info);
    VkSurfaceKHR surface = VK_NULL_HANDLE;
    SDL_Vulkan_CreateSurface(window, info.instance, &surface);
    return surface;
}

inline void DestroySurface(R1Instance* instance, VkSurfaceKHR surf) {
    auto vk_get_instance_proc_addr = GetVkGetInstanceProcAddr();
    if (!vk_get_instance_proc_addr) { return; }
    R1VKInstanceInfo info;
    R1_VK_GetInstanceInfo(instance, &info);
    auto vk_destroy_surface = reinterpret_cast<PFN_vkDestroySurfaceKHR>(
        vk_get_instance_proc_addr(info.instance, "vkDestroySurfaceKHR"));
    if (!vk_destroy_surface) { return; }
    vk_destroy_surface(info.instance, surf, nullptr);
}

template<class Derived>
class AppBase {
    bool            m_quit = false;
    SDL_Window*     m_window = nullptr;
    R1Instance*     m_instance = nullptr;
    R1Context*      m_ctx = nullptr;
    VkSurfaceKHR    m_surf = nullptr;
    R1Swapchain*    m_swc = nullptr;

protected:
    R1Scene*        m_scene = nullptr;

public:
    int Run();

protected:
    int Init() { return 0; }
    void ProcessEvent(SDL_Event) {}
    void Iterate() {}
    void TearDown() {}

private:
    int InitBase();
    void MainLoop();
    void TearDownBase();
};

template<class Derived>
inline int AppBase<Derived>::Run() {
    if (auto ret = InitBase()) {
        TearDownBase();
        return ret;
    }
    MainLoop();
    TearDownBase();
    return 0;
}

template<class Derived>
inline int AppBase<Derived>::InitBase() {
    auto impl = static_cast<Derived*>(this);
    SDL_Init(SDL_INIT_EVERYTHING);
    m_window = SDL_CreateWindow(
        impl->app_name,
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        1280, 720,
        SDL_WINDOW_RESIZABLE | SDL_WINDOW_VULKAN
    );

    m_instance = CreateInstance(impl->app_name);
    if (!m_instance) {
        std::cerr << "Failed to create renderer instance\n";
        return -1;
    }
    if (!R1_GetDeviceCount(m_instance)) {
        std::cerr << "Failed to create renderer context: no devices\n";
        return -1;
    }
    auto dev = R1_GetDevice(m_instance, 0);
    std::cout << "Running on " << R1_GetDeviceName(dev) << "\n";
    m_ctx = CreateContext(dev);
    if (!m_ctx) {
        std::cerr << "Failed to create renderer context\n";
        return -1;
    }
    m_surf = CreateSurface(m_instance, m_window);
    if (!m_surf) {
        std::cerr << "Failed to create window surface\n";
        return -1;
    }
    m_swc = [&] {
        auto surface_size_callback = [] (void* window, int* w, int* h) {
            SDL_GetWindowSize(reinterpret_cast<SDL_Window*>(window), w, h);
        };
        R1VKSwapchainCreateInfo create_info = {
            .surface = m_surf,
            .surface_size_callback = surface_size_callback,
            .usrptr = m_window,
            .present_mode = VK_PRESENT_MODE_FIFO_KHR,
        };
        return R1_VK_CreateSwapchain(m_ctx, &create_info);
    } ();
    if (!m_swc) {
        std::cerr << "Failed to create swapchain\n";
        return -1;
    }
    m_scene = R1_CreateScene(m_ctx);
    if (!m_scene) {
        std::cerr << "Failed to create scene\n";
        return -1;
    }
    return impl->Init();
}

template<class Derived>
inline void AppBase<Derived>::MainLoop() {
    auto impl = static_cast<Derived*>(this);
    while (!m_quit) {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                m_quit = true;
            }
            impl->ProcessEvent(e);
        }
        impl->Iterate();
        R1_DrawSceneToSwapchain(m_scene, m_swc);
    }
}

template<class Derived>
inline void AppBase<Derived>::TearDownBase() {
    auto impl = static_cast<Derived*>(this);
    impl->TearDown();

    R1_DestroyScene(m_scene);
    R1_DestroySwapchain(m_swc);
    DestroySurface(m_instance, m_surf);
    R1_DestroyContext(m_ctx);
    R1_DestroyInstance(m_instance);

    SDL_DestroyWindow(m_window);
    SDL_Quit();
}

template<size_t VertexCount, typename IndexType, size_t IndexCount>
    requires (IndexCount % 3 == 0)
constexpr std::array<glm::vec3, VertexCount> GenerateNormals(
    const std::array<glm::vec3, VertexCount>& positions,
    const std::array<IndexType, IndexCount>& indices
) {
    std::array<glm::vec3, VertexCount> normals;
    for (size_t t = 0; t < IndexCount; t += 3) {
        auto i0 = indices[t + 0];
        auto i1 = indices[t + 1];
        auto i2 = indices[t + 2];
        auto v0 = positions[i0];
        auto v1 = positions[i1];
        auto v2 = positions[i2];
        auto n = glm::normalize(glm::cross(v1 - v0, v2 - v0));
        normals[i0] = n;
        normals[i1] = n;
        normals[i2] = n;
    }
    return normals;
}
