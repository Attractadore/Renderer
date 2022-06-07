#include "R1Xlib.h"
#undef Always
#undef None
#include "Rendering/APIXlib.hpp"
#include "Types.hpp"

extern "C" {
R1Instance* R1_CreateInstanceXlib(::Display* dpy, int screen) {
    return new R1Instance{R1::Rendering::Xlib::CreateInstance(dpy, screen, {})};
}

R1Surface* R1_CreateSurfaceXlib(
    R1Instance* instance,
    ::Display* dpy,
    ::Window window,
    R1SurfaceSizeCallback c_size_cb,
    void* usrptr
) {
    return new R1Surface {
        R1::Rendering::Xlib::CreateSurface(
            instance->get().get(), dpy, window,
            R1::makeSizeCB(c_size_cb, usrptr)
        )
    };
}
}
