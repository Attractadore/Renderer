#include "R1/R1.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>

#include <iostream>

template<typename CreateInstance, typename CreateSurface>
int RunFromWindow(
    CreateInstance create_instance,
    CreateSurface create_surface,
    Uint32 flags = 0
) {
    SDL_Init(SDL_INIT_EVERYTHING);
    auto window = SDL_CreateWindow(
        "Triangle", 
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        640, 480,
        SDL_WINDOW_RESIZABLE | flags 
    );

    auto instance = create_instance(window);
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
    auto ctx = R1_CreateContext(dev);
    if (!ctx) {
        std::cerr << "Failed to create renderer context\n";
        return -1;
    }
    auto surf = create_surface(instance, window);
    if (!surf) {
        std::cerr << "Failed to create surface\n";
        return -1;
    }
    auto swc = R1_CreateSwapchain(ctx, surf);
    if (!swc) {
        std::cerr << "Failed to create swapchain\n";
    }
    auto scene = R1_CreateScene(ctx, swc);
    if (!scene) {
        std::cerr << "Failed to create scene\n";
        return -1;
    }

    bool quit = false;
    while (!quit) {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                quit = true;
            }
        }
        R1_DrawScene(scene);
    }

    R1_DestroyScene(scene);
    R1_DestroySwapchain(swc);
    R1_DestroySurface(surf);
    R1_DestroyContext(ctx);
    R1_DestroyInstance(instance);

    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

template<typename CreateInstance, typename CreateSurface>
int RunFromSysWMInfo(
    CreateInstance create_instance,
    CreateSurface create_surface,
    Uint32 flags = 0
) {
    auto create_instance_from_window =
    [&] (SDL_Window* window) {
        SDL_SysWMinfo info;
        SDL_VERSION(&info.version);
        if (!SDL_GetWindowWMInfo(window, &info)) {
            std::cerr << "Failed to retrieve WM info\n";
            return static_cast<R1Instance*>(nullptr);
        }
        return create_instance(info);
    };
    auto create_surface_from_window =
    [&] (R1Instance* instance, SDL_Window* window) {
        SDL_SysWMinfo info;
        SDL_VERSION(&info.version);
        if (!SDL_GetWindowWMInfo(window, &info)) {
            std::cerr << "Failed to retrieve WM info\n";
            return static_cast<R1Surface*>(nullptr);
        }
        return create_surface(instance, info, window);
    };
    return RunFromWindow(
        create_instance_from_window,
        create_surface_from_window,
        flags);
}
