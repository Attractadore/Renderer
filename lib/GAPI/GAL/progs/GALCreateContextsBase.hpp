#include "GAL/GAL.hpp"

#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>

#include <iostream>

void printDevices(R1::GAL::Instance instance) {
    auto dev_cnt = R1::GAL::GetDeviceCount(instance);
    for (size_t i = 0; i < dev_cnt; i++) {
        auto dev = R1::GAL::GetDevice(instance, i);
        const auto& dev_desc = R1::GAL::GetDeviceDescription(dev);
        std::cout << "Create context for device " << dev_desc.name << "\n";
        if (dev_desc.queue_families.empty()) {
            std::cout << "Device doesn't have any queues\n";
            continue;
        } else if (!dev_desc.wsi) {
            std::cout << "Device doesn't support presentation\n";
            continue;
        }
        R1::GAL::QueueConfig qcfg = {
            .id = dev_desc.queue_families[0].id,
            .count = 1,
        };
        auto ctx = R1::GAL::CreateContext(dev, {
            .queue_config = {&qcfg, 1},
            .wsi = true,
        });
        if (!ctx) {
            std::cout << "Failed to create context for device\n";
        } else {
            std::cout << "Success\n";
        }
        R1::GAL::DestroyContext(ctx);
    }
}

inline void RunFromWindow(
    std::function<R1::GAL::Instance (SDL_Window*, const R1::GAL::InstanceConfig&)> create_instance,
    const char* success, const char* error,
    Uint32 flags = 0u
) {
    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_SetHint(SDL_HINT_VIDEO_X11_NET_WM_BYPASS_COMPOSITOR, "0");
    auto window = SDL_CreateWindow(
        "SDL window",
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        640, 480,
        SDL_WINDOW_HIDDEN | flags
    );
    auto i = create_instance(window, {});
    if (i) {
        std::cout << success << "\n";
        printDevices(i);
    } else {
        std::cout << error << "\n";
    }
    R1::GAL::DestroyInstance(i);
    SDL_DestroyWindow(window);
    SDL_Quit();
};

inline void RunFromSysWM(
    std::function<R1::GAL::Instance (const SDL_SysWMinfo&, const R1::GAL::InstanceConfig&)> create_instance,
    const char* success, const char* error,
    Uint32 flags = 0u
) {
    auto create_instance_from_window =
    [&] (SDL_Window* window, const R1::GAL::InstanceConfig& config) {
        SDL_SysWMinfo info;
        SDL_VERSION(&info.version);
        SDL_GetWindowWMInfo(window, &info);
        return create_instance(info, {});
    };
    RunFromWindow(create_instance_from_window, success, error, flags);
}
