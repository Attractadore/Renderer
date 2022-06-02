#include "Vulkan/API.hpp"

#include <iostream>
#include <vector>

#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>

#if HAVE_X11
#include "Vulkan/APIXlib.hpp"
#ifdef HAVE_XLIB_XCB
#include <X11/Xlib-xcb.h>
#include "Vulkan/APIXCB.hpp"
#endif
#endif

void printDevices(R1::VK::Instance instance) {
    auto dev_cnt = R1::VK::GetDeviceCount(instance);
    for (size_t i = 0; i < dev_cnt; i++) {
        auto dev = R1::VK::GetDevice(instance, i);
        const auto& dev_desc = R1::VK::GetDeviceDescription(dev);
        std::cout << "Create context for device " << dev_desc.name << "\n";
        if (dev_desc.queue_families.empty()) {
            std::cout << "Device doesn't have any queues\n";
            continue;
        } else if (!dev_desc.wsi) {
            std::cout << "Device doesn't support presentation\n";
            continue;
        }
        R1::QueueConfig qcfg = {
            .id = dev_desc.queue_families[0].id,
            .count = 1,
        };
        auto ctx = R1::VK::CreateContext({
            .queue_config = {&qcfg, 1},
            .wsi = true,
        }, dev);
        if (!ctx) {
            std::cout << "Failed to create context for device\n";
        } else {
            std::cout << "Success\n";
        }
        R1::VK::DestroyContext(ctx);
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
        R1::VK::Instance i = nullptr;
        {
            std::cout << "Xlib devices:\n";
            auto i = R1::VK::CreateInstanceXlib({},
                info.info.x11.display
            );
            printDevices(i);
            R1::VK::DestroyInstance(i);
        }

#ifdef HAVE_XLIB_XCB
        {
            std::cout << "XCB devices:\n";
            auto i = R1::VK::CreateInstanceXCB({},
                XGetXCBConnection(info.info.x11.display)
            );
            printDevices(i);
            R1::VK::DestroyInstance(i);
        }
#endif
        break;
    }
#endif
    default: {
        std::cout << "DS is not X11!\n";
    }}

    SDL_DestroyWindow(window);
    SDL_Quit();
};
