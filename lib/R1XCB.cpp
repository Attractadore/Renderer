#include "R1XCB.h"
#include "GAPI/XCB.hpp"
#include "Types.hpp"

extern "C" {
R1Instance* R1_CreateInstanceXCB(xcb_connection_t* conn, int screen) {
    return new R1Instance{R1::GAPI::XCB::CreateInstance(conn, screen, {})};
}

R1Surface* R1_CreateSurfaceXCB(
    R1Instance* instance,
    xcb_connection_t* conn,
    xcb_window_t window,
    R1SurfaceSizeCallback c_size_cb,
    void* usrptr
) {
    return new R1Surface {
        R1::GAPI::XCB::CreateSurface(
            instance->get().get(), conn, window,
            R1::makeSizeCB(c_size_cb, usrptr)
        )
    };
}
}
