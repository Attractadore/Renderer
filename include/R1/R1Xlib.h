#pragma once
#ifdef __cplusplus
extern "C" {
#endif
#include "R1Types.h"

#include <X11/Xlib.h>

R1Instance* R1_CreateInstanceXlib(
    Display* dpy,
    int screen
);

R1Surface* R1_CreateSurfaceXlib(
    R1Instance* instance,
    Display* dpy,
    Window window,
    R1SurfaceSizeCallback size_cb,
    void* usrptr
);

#ifdef __cplusplus
}
#endif
