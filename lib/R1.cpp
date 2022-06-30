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
}
