#include "GLInstance.hpp"
#include "GLContext.hpp"

#include <iostream>
#include <vector>

#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>

#ifdef HAVE_XLIB_XCB
#include <X11/Xlib-xcb.h>
#endif

void printContexts(R1::GL::Instance& i) {
    auto dev_cnt = i.deviceCount();
    std::vector<R1::DeviceID> devs(dev_cnt);
    i.devices(devs);
    for (const auto& dev: devs) {
        std::cout << "Create context for " << i.deviceName(dev) << "\n";
        std::unique_ptr<R1::GL::Context> ctx{i.createContext(dev)};
    }
}

int main() {
    SDL_Init(SDL_INIT_EVERYTHING);

    SDL_SetHint(SDL_HINT_VIDEO_X11_NET_WM_BYPASS_COMPOSITOR, "0");
    auto window = SDL_CreateWindow(
        "SDL window",
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        640, 480,
        SDL_WINDOW_HIDDEN
    );
    SDL_SysWMinfo info;
    SDL_VERSION(&info.version);
    SDL_GetWindowWMInfo(window, &info);

    switch (info.subsystem) {
#if HAVE_X11
    case SDL_SYSWM_X11: {
        std::cout << "Xlib contexts:\n";
        auto i = R1::GL::Instance::makeXlibInstance(
            info.info.x11.display, info.info.x11.window
        );
        printContexts(i);

#ifdef HAVE_XLIB_XCB
        std::cout << "XCB contexts:\n";
        i = R1::GL::Instance::makeXCBInstance(
            XGetXCBConnection(info.info.x11.display), info.info.x11.window
        );
        printContexts(i);
#endif
        break;
    }
#endif
    default: {
        std::cout << "DS is not X11!\n";
    }}

    SDL_DestroyWindow(window);
    SDL_Quit();
}
