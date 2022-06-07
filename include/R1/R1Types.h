#pragma once
#ifdef __cplusplus
extern "C" {
#endif

typedef struct R1Instance   R1Instance;
typedef struct R1Device     R1Device;
typedef struct R1Context    R1Context;
typedef struct R1Surface    R1Surface;
typedef struct R1Swapchain  R1Swapchain;
typedef struct R1Scene      R1Scene;

typedef void (*R1SurfaceSizeCallback)(void* usrptr, int* width, int* height);

#ifdef __cplusplus
}
#endif
