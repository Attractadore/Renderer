#pragma once
#ifdef __cplusplus
extern "C" {
#endif
#include "R1Types.h"

#include <wayland-client.h>

R1Instance* R1_CreateInstanceWayland(
    struct wl_display* display
);

R1Surface* R1_CreateSurfaceWayland(
    R1Instance* instance,
    struct wl_display* display,
    struct wl_surface* surface,
    R1SurfaceSizeCallback size_cb,
    void* usrptr
);

#ifdef __cplusplus
}
#endif
