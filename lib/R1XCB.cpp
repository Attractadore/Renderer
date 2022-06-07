#include "R1XCB.h"
#include "Rendering/APIXCB.hpp"
#include "Types.hpp"

extern "C" {
R1Instance* R1_CreateInstanceXCB(xcb_connection_t* conn, int screen) {
    return new R1Instance{R1::Rendering::XCB::CreateInstance(conn, screen, {})};
}

R1Surface* R1_CreateSurfaceXlib(
    R1Instance* instance,
    xcb_connection_t* conn,
    xcb_window_t window,
    R1SurfaceSizeCallback c_size_cb,
    void* usrptr
) {
    return new R1Surface {
        R1::Rendering::XCB::CreateSurface(
            instance->get().get(), conn, window,
            R1::makeSizeCB(c_size_cb, usrptr)
        )
    };
}
}
