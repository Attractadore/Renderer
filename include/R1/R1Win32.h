#pragma once
#ifdef __cplusplus
extern "C" {
#endif
#include "R1Types.h"

#include <windows.h>

R1Instance* R1_CreateInstanceWin32(HDC hDc);

R1Surface* R1_CreateSurfaceWin32(
    R1Instance* instance,
    HWND hWnd,
    R1SurfaceSizeCallback size_cb,
    void* usrptr
);

#ifdef __cplusplus
}
#endif
