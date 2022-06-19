#pragma once
#ifdef __cplusplus
extern "C" {
#endif
#include "R1Types.h"

#include <SDL2/SDL_video.h>

R1Instance* R1_CreateInstanceSDL2(
    SDL_Window* window
);

R1Surface* R1_CreateSurfaceSDL2(
    R1Instance* instance,
    SDL_Window* window
);

SDL_WindowFlags R1_GetRequiredWindowFlagsSDL2();

#ifdef __cplusplus
}
#endif
