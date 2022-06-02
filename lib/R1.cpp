#include "Scene.hpp"

struct R1Instance: public R1::GAPI::Instance {
    using R1::GAPI::Instance::Instance;
};
struct R1Context: public R1::GAPI::Context {};
struct R1Scene: public R1::Scene {
    using R1::Scene::Scene;
};
struct R1Swapchain: public R1::GAPI::Swapchain {};

extern "C" {
R1Instance* R1_CreateXlibInstance(::Display* dpy, ::Window win) {
    return new R1Instance(dpy, win);
}

void R1_DestroyInstance(R1Instance* instance) {
    delete instance; 
}

size_t R1_GetInstanceDeviceCount(const R1Instance* instance) {
    return instance->deviceCount();
}

size_t R1_GetInstanceDevices(const R1Instance* instance, R1DeviceID* devices, size_t count) {
    return instance->devices({devices, count});
}

const char* R1_GetInstanceDeviceName(const R1Instance* instance, R1DeviceID dev) {
    return instance->deviceName(dev);
}

R1Context* R1_CreateContext(R1Instance* instance, R1DeviceID dev) {
    return static_cast<R1Context*>(instance->createContext(dev));
}

void R1_DestroyContext(R1Context* ctx) {
    delete ctx;
}

R1Swapchain* R1_CreateContextSwapchain(
    R1Context* ctx, R1SwapchainSizeCallback size_cb, void* usrptr, R1PresentMode pmode
) {
    return static_cast<R1Swapchain*>(ctx->createSwapchain(
        R1::SizeCallback{
            [=] () {
                int w = -1, h = -1;
                size_cb(&w, &h, usrptr);
                if (w < 0 or h < 0) {
                    throw std::runtime_error{"Failed to query surface size"};
                }
                return std::tuple<unsigned, unsigned>{w, h};
            }
        },
        static_cast<R1::PresentMode>(pmode)
    ));
}

R1Scene* R1_CreateScene(R1Context* ctx) {
    return new R1Scene(ctx);
}

void R1_DestroyScene(R1Scene* scene) {
    delete scene;
}

void R1_SceneDraw(R1Scene* scene) {
    scene->draw();
}
}
