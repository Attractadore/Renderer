#pragma once
#ifdef __cplusplus
extern "C" {
#endif

typedef struct R1Instance           R1Instance;
typedef struct R1Device             R1Device;
typedef struct R1Context            R1Context;
typedef struct R1Scene              R1Scene;
typedef struct R1Swapchain          R1Swapchain;
typedef struct R1MeshImpl*          R1Mesh;
typedef struct R1MeshInstanceImpl*  R1MeshInstance;

typedef void (*R1SurfaceSizeCallback)(void* usrptr, int* width, int* height);

typedef enum {
    R1_BACKEND_NONE,
    R1_BACKEND_VULKAN,
} R1Backend;

typedef enum {
    R1_PRESENT_MODE_IMMEDIATE,
    R1_PRESENT_MODE_MAILBOX,
    R1_PRESENT_MODE_FIFO,
    R1_PRESENT_MODE_FIFO_RELAXED,
} R1PresentMode;

#ifdef __cplusplus
}
#endif
