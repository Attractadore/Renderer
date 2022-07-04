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

typedef struct {
    const float*    vertices;
    unsigned        vertex_count;
} R1MeshConfig;

R1Mesh  R1_CreateMesh(R1Scene* scene, const R1MeshConfig* config);
void    R1_DestroyMesh(R1Scene* scene, R1Mesh mesh);

R1MeshInstance  R1_CreateMeshInstance(R1Scene* scene, R1Mesh mesh);
void            R1_DestroyMeshInstance(R1Scene* scene, R1MeshInstance mesh_instance);

#ifdef __cplusplus
}
#endif
