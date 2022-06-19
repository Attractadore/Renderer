#pragma once
#include "GAL/GALSDL2.hpp"
#include "Instance.hpp"
#include "Swapchain.hpp"

namespace R1::GAPI::SDL2 {
inline Instance CreateInstance(
    SDL_Window* window,
    const GAL::InstanceConfig& config
) {
    HInstance instance{GAL::SDL2::CreateInstance(window, config)};
    return Instance{std::move(instance)};
}

inline Surface CreateSurface(
    GAL::Instance instance,
    SDL_Window* window
) {
    HSurface surface{GAL::SDL2::CreateSurface(instance, window)};
    return Surface{std::move(surface), GAL::SDL2::CreateWindowSizeCallback(window)};
}
}
