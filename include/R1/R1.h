#pragma once
#ifdef __cplusplus
extern "C" {
#endif
#include "R1Types.h"

#include <stddef.h>

void            R1_DestroyInstance(R1Instance* instance);

size_t          R1_GetDeviceCount(const R1Instance* instance);
R1Device*       R1_GetDevice(R1Instance* instance, size_t idx);
const char*     R1_GetDeviceName(R1Device* device);

R1Context*      R1_CreateContext(R1Device* device);
void            R1_DestroyContext(R1Context* ctx);

// TODO: add queries for surface properties
void            R1_DestroySurface(R1Surface* surface);

R1Swapchain*    R1_CreateSwapchain(R1Context* ctx, R1Surface* surface);
void            R1_DestroySwapchain(R1Swapchain* swapchain);

R1Scene*        R1_CreateScene(R1Context* cxt, R1Swapchain* swapchain);
void            R1_DestroyScene(R1Scene* scene);
void            R1_DrawScene(R1Scene* scene);

#ifdef __cplusplus
}
#endif
