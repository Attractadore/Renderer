#pragma once
#include "R1Types.h"

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

void            R1_DestroyInstance(R1Instance* instance);

size_t          R1_GetDeviceCount(const R1Instance* instance);
R1Device*       R1_GetDevice(R1Instance* instance, size_t idx);
const char*     R1_GetDeviceName(R1Device* device);

void            R1_DestroyContext(R1Context* ctx);

void            R1_DestroySwapchain(R1Swapchain* swapchain);

R1Scene*        R1_CreateScene(R1Context* cxt);
void            R1_DestroyScene(R1Scene* scene);

size_t          R1_GetSceneOutputImageCount(R1Scene* scene);
void            R1_DrawSceneToSwapchain(R1Scene* scene, R1Swapchain* swapchain);

#ifdef __cplusplus
}
#endif
