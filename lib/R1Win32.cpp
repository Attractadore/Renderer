#include "R1Win32.h"
#include "GAPI/Win32.hpp"
#include "Types.hpp"

extern "C" {
R1Instance* R1_CreateInstanceWin32(HDC hDc) {
    return new R1Instance{R1::GAPI::Win32::CreateInstance(hDc, {})};
}

R1Surface* R1_CreateSurfaceWin32(
    R1Instance* instance,
    HWND hWnd,
    R1SurfaceSizeCallback c_size_cb,
    void* usrptr
) {
    return new R1Surface {
        R1::GAPI::Win32::CreateSurface(
            instance->get().get(), hWnd,
            R1::makeSizeCB(c_size_cb, usrptr)
        )
    };
}
}
