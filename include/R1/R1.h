#pragma once
#ifdef __cplusplus
extern "C" {
#endif
#include "R1Types.h"
#include "R1PresentModes.h"

#include <X11/Xlib.h>
#include <xcb/xcb.h>

R1Instance*     R1_CreateXlibInstance(Display* dpy, Window win);
R1Instance*     R1_CreateXCBInstance(xcb_connection_t* c, xcb_window_t win);
void            R1_DestroyInstance(R1Instance* instance);

size_t          R1_GetInstanceDeviceCount(const R1Instance* instance);
size_t          R1_GetInstanceDevices(const R1Instance* instance, R1DeviceID* devs, size_t cnt);
const char*     R1_GetInstanceDeviceName(const R1Instance* instance, R1DeviceID dev);

R1Context*      R1_CreateContext(R1Instance* instance, R1DeviceID dev);
void            R1_DestroyContext(R1Context* ctx);
size_t          R1_GetSwapchainPresentModeCount(const R1Context* ctx);
size_t          R1_GetSwapchainPresentModes(const R1Context* ctx, R1PresentMode* pmodes, size_t cnt);
R1Swapchain*    R1_CreateContextSwapchain(R1Context* ctx, R1SwapchainSizeCallback size_cb, void* usrptr, R1PresentMode pmode);

R1Scene*        R1_CreateScene(R1Context* cxt);
void            R1_DestroyScene(R1Scene* scene);
void            R1_SceneDraw(R1Scene* scene);

#ifdef __cplusplus
}
#endif
