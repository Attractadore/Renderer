#include "R1.h"
#include "R1Impl.hpp"
#include "Scene.hpp"
#include "Instance.hpp"
#include "Swapchain.hpp"

#include <glm/gtc/type_ptr.hpp>

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
    return R1::ToPublic(scene->CreateMesh({
        .vertices = {config->vertices, 3 * config->vertex_count}
    }));
}

void R1_DestroyMesh(R1Scene* scene, R1Mesh mesh) {
    scene->DestroyMesh(R1::ToPrivate(mesh));
}

R1MeshInstance R1_CreateMeshInstance(R1Scene* scene, const R1MeshInstanceConfig* config) {
    return R1::ToPublic(scene->CreateMeshInstance({
        .transform = glm::make_mat4(config->transform),
        .mesh = R1::ToPrivate(config->mesh),
    }));
}

void R1_DestroyMeshInstance(R1Scene* scene, R1MeshInstance mesh_instance) {
    scene->DestroyMeshInstance(
        R1::ToPrivate(mesh_instance));
}

void R1_SetMeshInstanceTransform(R1Scene* scene, R1MeshInstance mesh_instance, const float transform[16]) {
    scene->GetMeshInstanceTransform(R1::ToPrivate(mesh_instance)) =
        glm::make_mat4(transform);
}

void R1_GetMeshInstanceTransform(const R1Scene* scene, R1MeshInstance mesh_instance, float transform[16]) {
    const auto& mat =
        scene->GetMeshInstanceTransform(R1::ToPrivate(mesh_instance));
    std::memcpy(transform, &mat, sizeof(mat));
}

void R1_SetCamera(R1Scene* scene, const R1CameraConfig* config) {
    auto& camera = scene->GetCamera();
    camera.position = glm::make_vec3(config->position);
    camera.direction = glm::make_vec3(config->direction);
    camera.up = glm::make_vec3(config->up);
    camera.fov = config->fov;
}

void R1_SetCameraPosition(R1Scene* scene, const float position[3]) {
    scene->GetCamera().position = glm::make_vec3(position);
}

void R1_SetCameraDirection(R1Scene* scene, const float direction[3]) {
    scene->GetCamera().direction = glm::make_vec3(direction);
}

void R1_SetCameraUp(R1Scene* scene, const float up[3]) {
    scene->GetCamera().up = glm::make_vec3(up);
}

void R1_SetCameraFOV(R1Scene* scene, float fov) {
    scene->GetCamera().fov = glm::min(fov, glm::radians(170.0f));
}

void R1_GetCamera(const R1Scene* scene, R1CameraConfig* config) {
    const auto& camera = scene->GetCamera();
    std::memcpy(config->position, &camera.position, sizeof(camera.position));
    std::memcpy(config->direction, &camera.direction, sizeof(camera.direction));
    std::memcpy(config->up, &camera.up, sizeof(camera.up));
    config->fov = camera.fov;
}
}
