#pragma once
#include "Instance.hpp"
#include "Swapchain.hpp"

#include <SDL2/SDL_video.h>

namespace R1::GAL::SDL2 {
Instance CreateInstance(
    SDL_Window* window, const InstanceConfig& config
);

Surface CreateSurface(
    Instance instance, SDL_Window* window 
);

SDL_WindowFlags GetRequiredWindowFlags();

SurfaceSizeCallback CreateWindowSizeCallback(SDL_Window* window);
}

