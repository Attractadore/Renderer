#pragma once
#include "GAL/Instance.hpp"

#include <EGL/egl.h>

namespace R1::GAL {
struct OpenGLInstance {
    EGLDisplay                  dpy;
    std::vector<OpenGLAdapter>  adapters;
};

struct OpenGLDeviceDescription {
    DeviceDescription common;
};

struct OpenGLAdapter {
    OpenGLDeviceDescription description;
};

Instance CreateInstanceFromEGLDisplay(EGLDisplay dpy);
}
