#include "R1/R1.h"

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
    auto win = info.info.x11.window;
    return R1_CreateXlibInstance(dpy, win);
}

int main() {
    SDL_Init(SDL_INIT_EVERYTHING);
    auto window = SDL_CreateWindow(
        "Triangle", 
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        640, 480,
        SDL_WINDOW_RESIZABLE
    );
    auto get_window_size = [] (int32_t* w, int32_t* h, void* usrptr) {
        auto window = reinterpret_cast<SDL_Window*>(usrptr);
        SDL_GetWindowSize(window, w, h);
    };

    auto i = createX11Instance(window);
    if (!i) {
        std::cerr << "Failed to create X11 renderer instance\n";
        return -1;
    }
    R1DeviceID dev;
    if (!R1_GetInstanceDevices(i, &dev, 1)) {
        std::cerr << "Failed to create renderer context\n";
        return -1;
    }
    std::cout << "Running on " << R1_GetInstanceDeviceName(i, dev) << "\n";
    auto c = R1_CreateContext(i, dev);
             R1_CreateContextSwapchain(c, get_window_size, window, R1_PRESENT_MODE_VSYNC);
    auto s = R1_CreateScene(c);

    bool quit = false;
    while (!quit) {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                quit = true;
            }
        }
        R1_SceneDraw(s);
    }

    R1_DestroyScene(s);
    R1_DestroyContext(c);
    R1_DestroyInstance(i);

    SDL_DestroyWindow(window);
    SDL_Quit();
}
