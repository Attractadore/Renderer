#include "R1Wayland.h"
#include "GAPI/Wayland.hpp"
#include "Types.hpp"

extern "C" {
R1Instance* R1_CreateInstanceWayland(struct wl_display* display) {
    return new R1Instance{R1::GAPI::Wayland::CreateInstance(display, {})};
}

R1Surface* R1_CreateSurfaceWayland(
    R1Instance* instance,
    struct wl_display* display,
    struct wl_surface* surface,
    R1SurfaceSizeCallback c_size_cb,
    void* usrptr
) {
    return new R1Surface {
        R1::GAPI::Wayland::CreateSurface(
            instance->get().get(), display, surface,
            R1::makeSizeCB(c_size_cb, usrptr)
        )
    };
}
}
