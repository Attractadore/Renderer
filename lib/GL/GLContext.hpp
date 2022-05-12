#pragma once
#include "Common/Interfaces.hpp"
#include "EGLRAII.hpp"
#include "GLTypes.hpp"

#include <glad/gl.h>

namespace R1::GL {
class Context {
    EGL::Context  m_context;
    EGLSurface    m_surface;
    GladGLContext m_gl;

public:
    Context(EGLDisplay dpy, EGLSurface surf);

private:
    void makeCurrent();
};
static_assert(IContext<Context>);
}
