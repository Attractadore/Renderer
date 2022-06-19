#include "R1/R1.h"
#include "R1/R1Wayland.h"
#include "R1/R1Xlib.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>

#include <iostream>

R1Instance* createInstance(SDL_Window* window) {
    SDL_SysWMinfo info;
    SDL_VERSION(&info.version);
    SDL_GetWindowWMInfo(window, &info);
    switch(info.subsystem) {
        case SDL_SYSWM_X11: {
            std::cout << "Create X11 instance\n";
            auto dpy = info.info.x11.display;
            return R1_CreateInstanceXlib(dpy, 0);
        } case SDL_SYSWM_WAYLAND: {
            std::cout << "Create Wayland instance\n";
            auto display = info.info.wl.display;
            return R1_CreateInstanceWayland(display);
        } default:
            return nullptr;
    }
}

R1Surface* createSurface(R1Instance* instance, SDL_Window* window) {
    SDL_SysWMinfo info;
    SDL_VERSION(&info.version);
    SDL_GetWindowWMInfo(window, &info);
    switch(info.subsystem) {
        case SDL_SYSWM_X11: {
            std::cout << "Create X11 surface\n";
            auto dpy = info.info.x11.display;
            auto win = info.info.x11.window;
            return R1_CreateSurfaceXlib(
                instance, dpy, win,
                [] (void* usrptr, int* w, int* h) {
                    SDL_GetWindowSize(reinterpret_cast<SDL_Window*>(usrptr), w, h);
                },
                window);
        } case SDL_SYSWM_WAYLAND: {
            std::cout << "Create Wayland surface\n";
            auto display = info.info.wl.display;
            auto surface = info.info.wl.surface;
            return R1_CreateSurfaceWayland(
                instance, display, surface,
                [] (void* usrptr, int* w, int* h) {
                    SDL_GetWindowSize(reinterpret_cast<SDL_Window*>(usrptr), w, h);
                },
                window);
        } default:
            return nullptr;
    }
}

int main() {
    SDL_Init(SDL_INIT_EVERYTHING);
    auto window = SDL_CreateWindow(
        "Triangle", 
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        640, 480,
        SDL_WINDOW_RESIZABLE
    );

    auto instance = createInstance(window);
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
    auto surf = createSurface(instance, window);
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
}
