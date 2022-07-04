#include "R1.h"
#include "R1Impl.hpp"
#include "Scene.hpp"
#include "Instance.hpp"
#include "Swapchain.hpp"

extern "C" {
size_t R1_GetDeviceCount(const R1Instance* instance) {
    return instance->GetDeviceCount();
}

R1Device* R1_GetDevice(R1Instance* instance, size_t idx) {
    return R1::ToPublic(&instance->GetDevice(idx));
}

const char* R1_GetDeviceName(R1Device* device) {
    return R1::ToPrivate(device)->GetName().c_str();
}

size_t R1_GetSceneOutputImageCount(R1Scene* scene) {
    return scene->GetOutputImageCount();
}

R1Mesh R1_CreateMesh(R1Scene* scene, const R1MeshConfig* config) {
    return R1::ToPublic(scene->CreateMesh(
        {.vertices = {config->vertices, config->vertex_count}}));
}

void R1_DestroyMesh(R1Scene* scene, R1Mesh mesh) {
    scene->DestroyMesh(R1::ToPrivate(mesh));
}

#if 0
R1MeshInstance R1_CreateMeshInstance(R1Scene* scene, R1Mesh mesh) {
    return R1::ToPublic(
        scene->CreateMeshInstance(R1::ToPrivate(mesh)));
}

void R1_DestroyMeshInstance(R1Scene* scene, R1MeshInstance mesh_instance) {
    scene->DestroyMeshInstance(
        R1::ToPrivate(mesh_instance));
}
#endif
}
