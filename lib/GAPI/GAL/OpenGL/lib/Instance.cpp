#include "EGLRAII.hpp"
#include "GLUtil.hpp"
#include "Instance.hpp"
#include "Common/Array.hpp"

#include <EGL/eglext.h>
#include <glad/gl.h>

namespace R1::GAL {
namespace {
void GetInstanceAdapters(Instance instance) {
    static constexpr auto attribs = make_array<EGLAttrib>();
    auto dpy = instance->dpy;
    assert(dpy);
    ThrowIfFailed(
        eglBindAPI(EGL_OPENGL_API),
        "EGL: Failed to bind OpenGL API");
    auto ctx = egl::Context{dpy,
        eglCreateContext(dpy, EGL_NO_CONFIG_KHR, EGL_NO_CONTEXT, nullptr)};
    ThrowIfFailed(ctx.get(), "No context");
    if (!ctx) {
        return;
    }
    ThrowIfFailed(
        eglMakeCurrent(dpy, EGL_NO_SURFACE, EGL_NO_SURFACE, ctx.get()),
        "EGL: Failed to make context current");
    GladGLContext gl;
    if (auto ver = gladLoadGLContext(&gl, eglGetProcAddress); ver != GLAD_MAKE_VERSION(4, 6)) {
        return;
    }
    auto name = reinterpret_cast<const char*>(gl.GetString(GL_RENDERER));
    ThrowIfFailed(name, "OpenGL: Failed to query renderer name");
    OpenGLAdapter adapter = {
        .description = { .common = {
            .name = name,
            .type = DeviceType::Unknown,
            .queue_families = {{
                .id = static_cast<QueueFamily::ID>(0),
                .capabilities =
                    QueueCapability::Graphics |
                    QueueCapability::Compute |
                    QueueCapability::Transfer,
                .count = 1,
            }},
            .wsi = true,
            },
        },
    };
    instance->adapters.emplace_back(std::move(adapter));
}
}

Instance CreateInstanceFromEGLDisplay(EGLDisplay dpy) {
    auto instance = std::make_unique<OpenGLInstance>();
    instance->dpy = dpy;
    GetInstanceAdapters(instance.get());
    return instance.release();
};

void DestroyInstance(Instance instance) {
    delete instance;
}

size_t GetDeviceCount(Instance instance) {
    return instance->adapters.size();
}

Device GetDevice(Instance instance, size_t idx) {
    return &instance->adapters[idx];
}

const DeviceDescription& GetDeviceDescription(Device device) {
    return device->description.common;
}
}
