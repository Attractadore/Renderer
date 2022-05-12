#pragma once
#ifdef __cplusplus
extern "C" {
#endif

typedef struct R1Instance   R1Instance;
typedef unsigned            R1DeviceID;
typedef struct R1Context    R1Context;
typedef void (*R1SwapchainSizeCallback)(int* width_out, int* height_out, void* usrptr);
typedef struct R1Swapchain  R1Swapchain;
typedef struct R1Scene      R1Scene;

#ifdef __cplusplus
}
#endif
