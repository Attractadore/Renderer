#include "R1.h"
#include "Types.hpp"

namespace {
R1Device* ToPublic(R1::Rendering::Device* device) {
    return reinterpret_cast<R1Device*>(device);
}

R1::Rendering::Device* ToPrivate(R1Device* device) {
    return reinterpret_cast<R1::Rendering::Device*>(device);
}
};

extern "C" {
void R1_DestroyInstance(R1Instance* instance) {
    delete instance;
}

size_t R1_GetDeviceCount(const R1Instance* instance) {
    return instance->GetDeviceCount();
}

R1Device* R1_GetDevice(R1Instance* instance, size_t idx) {
    return ToPublic(&instance->GetDevice(idx));
}

const char* R1_GetDeviceName(R1Device* device) {
    return ToPrivate(device)->GetName().c_str();
}

R1Context* R1_CreateContext(R1Device* device) {
    return new R1Context{*ToPrivate(device)};
}

void R1_DestroyContext(R1Context* ctx) {
    delete ctx; 
}

void R1_DestroySurface(R1Surface* surface){
    delete surface;
}

R1Swapchain* R1_CreateSwapchain(R1Context* ctx, R1Surface* surf) {
    return new R1Swapchain{ctx->get(), surf->get()};
}

void R1_DestroySwapchain(R1Swapchain* swapchain) {
    delete swapchain;
}

R1Scene* R1_CreateScene(R1Context* ctx) {
    return new R1Scene{*ctx};
}

void R1_DrawScene(R1Scene* scene, R1Swapchain* swapchain) {
    scene->Draw(*swapchain);
}

void R1_DestroyScene(R1Scene* scene) {
    delete scene;
}
}
