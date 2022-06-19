#include "DrawTriangleBase.hpp"
#include "R1/R1SDL2.h"

#include <iostream>

int main() {
    return RunFromWindow(
        R1_CreateInstanceSDL2,
        R1_CreateSurfaceSDL2,
        R1_GetRequiredWindowFlagsSDL2());
}
