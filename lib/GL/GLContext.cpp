#include "GLContext.hpp"

#include <array>
#include <stdexcept>

namespace R1::GL {
namespace {
constexpr std::array ctx_attrib_list = {
     EGL_CONTEXT_MAJOR_VERSION, 4,
     EGL_CONTEXT_MINOR_VERSION, 6,
     EGL_CONTEXT_OPENGL_PROFILE_MASK,  EGL_CONTEXT_OPENGL_CORE_PROFILE_BIT,
     EGL_CONTEXT_OPENGL_DEBUG, R1_DEBUG ? EGL_TRUE : EGL_FALSE,
     EGL_NONE,
};
}

Context::Context(EGLDisplay dpy, EGLSurface surf):
    m_context{dpy, ctx_attrib_list.data()}, m_surface{surf}
{
    makeCurrent();
    if (!gladLoadGLContext(&m_gl, eglGetProcAddress)) {
        throw std::runtime_error{"Failed to load OpenGL function pointers"};
    }
}

void Context::makeCurrent() {
    if (!eglMakeCurrent(m_context.get_display(), m_surface, m_surface, m_context.get())) {
        throw std::runtime_error{"Failed to make OpenGL context current"};
    }
}
}
