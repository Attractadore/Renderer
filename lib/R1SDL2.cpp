#include "R1SDL2.h"
#include "GAPI/SDL2.hpp"
#include "Types.hpp"

extern "C" {
R1Instance* R1_CreateInstanceSDL2(SDL_Window* window) {
    return new R1Instance{R1::GAPI::SDL2::CreateInstance(window, {})};
}

R1Surface* R1_CreateSurfaceSDL2(
    R1Instance* instance,
    SDL_Window* window
) {
    return new R1Surface {
        R1::GAPI::SDL2::CreateSurface(
            instance->get().get(), window
        )
    };
}

SDL_WindowFlags R1_GetRequiredWindowFlagsSDL2() {
    return R1::GAL::SDL2::GetRequiredWindowFlags();
}
}
