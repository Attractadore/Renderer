#include "R1/R1.h"
#include "R1/R1Xlib.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>

#include <iostream>

R1Instance* createX11Instance(SDL_Window* window) {
    SDL_SysWMinfo info;
    SDL_VERSION(&info.version);
    SDL_GetWindowWMInfo(window, &info);
    if (info.subsystem != SDL_SYSWM_X11) {
        return nullptr;
    }

    auto dpy = info.info.x11.display;
    return R1_CreateInstanceXlib(dpy, 0);
}

R1Surface* createX11Surface(R1Instance* instance, SDL_Window* window) {
    SDL_SysWMinfo info;
    SDL_VERSION(&info.version);
    SDL_GetWindowWMInfo(window, &info);
    if (info.subsystem != SDL_SYSWM_X11) {
        return nullptr;
    }

    auto dpy = info.info.x11.display;
    auto win = info.info.x11.window;
    return R1_CreateSurfaceXlib(
        instance, dpy, win,
        reinterpret_cast<R1SurfaceSizeCallback>(SDL_GetWindowSize),
        window);
}

int main() {
    SDL_Init(SDL_INIT_EVERYTHING);
    auto window = SDL_CreateWindow(
        "Triangle", 
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        640, 480,
        SDL_WINDOW_RESIZABLE
    );

    auto instance = createX11Instance(window);
    if (!instance) {
        std::cerr << "Failed to create X11 renderer instance\n";
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
    auto surf = createX11Surface(instance, window);
    if (!surf) {
        std::cerr << "Failed to create X11 surface\n";
        return -1;
    }
#if 0
    auto swc = R1_CreateSwapchain(ctx, surf);
    if (!swc) {
        std::cerr << "Failed to create swapchain\n";
    }
    auto scene = R1_CreateScene(ctx);
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
        R1_SceneDraw(scene, swc);
    }

    R1_DestroyScene(scene);
    R1_DestroySwapchain(swc);
#endif
    R1_DestroySurface(surf);
    R1_DestroyContext(ctx);
    R1_DestroyInstance(instance);

    SDL_DestroyWindow(window);
    SDL_Quit();
}
