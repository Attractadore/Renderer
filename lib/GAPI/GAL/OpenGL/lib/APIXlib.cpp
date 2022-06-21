#include "Common/Array.hpp"
#include "GAL/GALXlib.hpp"
#include "GLUtil.hpp"
#include "Instance.hpp"

#include <EGL/eglext.h>

namespace R1::GAL::Xlib {
Instance CreateInstance(
    ::Display* dpy,
    int screen,
    const InstanceConfig& config
) {
    auto attribs = make_array<EGLAttrib>(
        EGL_PLATFORM_X11_SCREEN_KHR,
        screen,
        EGL_NONE);
    auto egl_dpy = eglGetPlatformDisplay(EGL_PLATFORM_X11_KHR, dpy, attribs.data());
    ThrowIfFailed(egl_dpy, "EGL: Failed to get Xlib display");
    ThrowIfFailed(
        eglInitialize(egl_dpy, nullptr, nullptr),
        "EGL: Failed to initialize Xlib display");
    return CreateInstanceFromEGLDisplay(egl_dpy);
}
}
