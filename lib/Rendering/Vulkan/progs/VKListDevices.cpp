#include "Vulkan/API.hpp"
#include "Vulkan/APIXlib.hpp"

#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>

#include <iostream>

const char* GetDeviceTypeStr(R1::DeviceType dev_type) {
    using enum R1::DeviceType;
    switch(dev_type) {
        case CPU:
            return "CPU";
        case IntegratedGPU:
            return "iGPU";
        case DiscreteGPU:
            return "dGPU";
        case VirtualGPU:
            return "virtual GPU";
        default:
            return "unknown";
    }
}

void printDevices(R1::VK::Instance instance) {
    auto dev_cnt = R1::VK::GetDeviceCount(instance);
    for (size_t i = 0; i < dev_cnt; i++) {
        auto dev = R1::VK::GetDevice(instance, i);
        const auto& dev_desc = R1::VK::GetDeviceDescription(dev);
        std::cout << GetDeviceTypeStr(dev_desc.type) << " device " << dev_desc.name << "\n";
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
    case SDL_SYSWM_X11: {
        auto i = R1::VK::CreateInstanceXlib(
            info.info.x11.display, 0, {}
        );
        printDevices(i);
        R1::VK::DestroyInstance(i);
        break;
    }
    default: {
        std::cout << "Window system is not X11!\n";
    }}

    SDL_DestroyWindow(window);
    SDL_Quit();
};
