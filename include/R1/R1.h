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

typedef enum {
    R1_INDEX_FORMAT_16,
    R1_INDEX_FORMAT_32,
} R1IndexFormat;

typedef struct {
    const float*    positions;
    const float*    normals;
    unsigned        vertex_count;
    R1IndexFormat   index_format;
    const void*     indices;
    unsigned        index_count;
} R1MeshConfig;

R1Mesh  R1_CreateMesh(R1Scene* scene, const R1MeshConfig* config);
void    R1_DestroyMesh(R1Scene* scene, R1Mesh mesh);

typedef struct {
    float   transform[16];
    R1Mesh  mesh;
} R1MeshInstanceConfig;

R1MeshInstance  R1_CreateMeshInstance(R1Scene* scene, const R1MeshInstanceConfig* config);
void            R1_DestroyMeshInstance(R1Scene* scene, R1MeshInstance mesh_instance);
void            R1_SetMeshInstanceTransform(R1Scene* scene, R1MeshInstance mesh_instance, const float transform[C_ARRAY_STATIC 16]);
void            R1_GetMeshInstanceTransform(const R1Scene* scene, R1MeshInstance mesh_instance, float transform[C_ARRAY_STATIC 16]);

typedef struct {
    float position[3];
    float direction[3];
    float up[3];
    float fov;
} R1CameraConfig;

void            R1_SetCamera(R1Scene* scene, const R1CameraConfig* config);
void            R1_SetCameraPosition(R1Scene* scene, const float position[C_ARRAY_STATIC 3]);
void            R1_SetCameraDirection(R1Scene* scene, const float direction[C_ARRAY_STATIC 3]);
void            R1_SetCameraUp(R1Scene* scene, const float up[C_ARRAY_STATIC 3]);
void            R1_SetCameraFOV(R1Scene* scene, float fov);
void            R1_GetCamera(const R1Scene* scene, R1CameraConfig* config);

#ifdef __cplusplus
}
#endif
