#pragma once
#ifdef __cplusplus
extern "C" {
#endif
#include "R1Types.h"

#include <xcb/xcb.h>

R1Instance* R1_CreateInstanceXCB(
    xcb_connection_t* conn,
    int screen
);

R1Surface* R1_CreateSurfaceXCB(
    R1Instance* instance,
    xcb_connection_t* conn,
    xcb_window_t window,
    R1SurfaceSizeCallback size_cb,
    void* usrptr
);

#ifdef __cplusplus
}
#endif
