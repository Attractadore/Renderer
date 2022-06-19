#include "GAL/GALSDL2.hpp"
#include "InstanceImpl.hpp"
#include "SwapchainImpl.hpp"
#include "VKUtil.hpp"

#include <SDL2/SDL_vulkan.h>

namespace R1::GAL::SDL2 {
Instance CreateInstance(
    SDL_Window*,
    const InstanceConfig& config
) {
    auto get_instance_extensions = [] (uint32_t* cnt, const char** exts) {
        auto r = SDL_Vulkan_GetInstanceExtensions(nullptr, cnt, exts);
        return r ? VK_SUCCESS: VK_ERROR_UNKNOWN;
    };
    auto exts = Enumerate<const char*>(get_instance_extensions);
    auto instance = CreateVkInstanceWithExtensions(config, exts);
    if (!instance) {
        throw std::runtime_error{"Vulkan: Failed to create SDL2 instance"};
    }
    return CreateInstanceFromVK(std::move(instance));
}

Surface CreateSurface(
    Instance instance,
    SDL_Window* window
) {
    auto create_surface = [] (
        SDL_Window* window,
        VkInstance instance,
        VkSurfaceKHR* surface
    ) {
        auto r = SDL_Vulkan_CreateSurface(window, instance, surface);
        return r ? VK_SUCCESS: VK_ERROR_UNKNOWN;
    };

    VkSurfaceKHR surface;
    ThrowIfFailed(
        create_surface(window, instance->instance.get(), &surface),
        "Vulkan: Failed to create SDL2 surface");
    return CreateSurfaceFromHandle(
        instance, {instance->instance.get(), surface});
}

SDL_WindowFlags GetRequiredWindowFlags() {
    return SDL_WINDOW_VULKAN;
}

SurfaceSizeCallback CreateWindowSizeCallback(SDL_Window* window) {
    return [=] {
        int w, h;
        SDL_Vulkan_GetDrawableSize(window, &w, &h);
        if (w < 0 or h < 0) {
            throw std::runtime_error{
                "Vulkan: Failed to get SDL2 window size"};
        }
        return std::tuple<unsigned, unsigned>{w, h};
    };
}
}
