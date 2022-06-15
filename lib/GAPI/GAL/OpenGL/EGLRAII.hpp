#pragma once
#include "Common/Array.hpp"
#include "Common/Handle.hpp"

#include <stdexcept>

#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <X11/Xlib.h>
#include <xcb/xcb.h>

namespace R1::EGL {
namespace Detail {
struct DisplayDeleter {
    void operator()(EGLDisplay dpy) const { eglTerminate(dpy); }
};

class WithDisplayDeleterBase {
protected:
    EGLDisplay m_dpy;

public:
    explicit WithDisplayDeleterBase(EGLDisplay dpy): m_dpy{dpy} {}

    EGLDisplay get_display() const { return m_dpy; }
};

struct ContextDeleter: WithDisplayDeleterBase {
    explicit ContextDeleter(EGLDisplay dpy): WithDisplayDeleterBase{dpy} {}

    void operator()(EGLContext ctx) const {
        if (eglGetCurrentContext() == ctx) {
            eglMakeCurrent(
                m_dpy,
                EGL_NO_SURFACE, EGL_NO_SURFACE,
                EGL_NO_CONTEXT
            );
        }
        eglDestroyContext(m_dpy, ctx);
    }
};

struct SurfaceDeleter: WithDisplayDeleterBase {
    explicit SurfaceDeleter(EGLDisplay dpy): WithDisplayDeleterBase{dpy} {}

    void operator()(EGLSurface surf) const {
        eglDestroySurface(m_dpy, surf);
    }
};

using DisplayBase = Handle<EGLDisplay, DisplayDeleter>;
using ContextBase = Handle<EGLContext, ContextDeleter>;
using SurfaceBase = Handle<EGLSurface, SurfaceDeleter>;
}

class Display: public Detail::DisplayBase {
    Display(EGLDisplay dpy): Detail::DisplayBase {
        [&] {
            if (!eglInitialize(dpy, nullptr, nullptr)) {
                throw std::runtime_error{"Failed to initialize EGL display"};
            }
            return dpy;
        } ()
    } {}

public:
    static Display makeXlibDisplay(::Display* xdpy, int screen) {
        const auto attrib_list = make_array<EGLAttrib> (
            EGL_PLATFORM_X11_SCREEN_KHR, screen,
            EGL_NONE
        );
        auto dpy = eglGetPlatformDisplay(EGL_PLATFORM_X11_KHR, xdpy, attrib_list.data());
        if (!dpy) {
            throw std::runtime_error{"Failed to retrieve Xlib display"};
        }
        return Display{dpy};
    }

    static Display makeXCBDisplay(xcb_connection_t* con, int screen) {
        const auto attrib_list = make_array<EGLAttrib> (
            EGL_PLATFORM_XCB_SCREEN_EXT, screen,
            EGL_NONE
        );
        auto dpy = eglGetPlatformDisplay(EGL_PLATFORM_XCB_EXT, con, attrib_list.data());
        if (!dpy) {
            throw std::runtime_error{"Failed to retrieve XCB display"};
        }
        return Display{dpy};
    }

    using Detail::DisplayBase::get;
};

class Context: Detail::ContextBase {
public:
    explicit Context(EGLDisplay dpy, const EGLint* attrib_list = nullptr): Detail::ContextBase {
        [&] {
            if (!eglBindAPI(EGL_OPENGL_API)) {
                throw std::runtime_error{"Failed to bind EGL API"};
            }
            auto ctx = eglCreateContext(dpy, EGL_NO_CONFIG_KHR, EGL_NO_CONTEXT, attrib_list);
            if (!ctx) {
                throw std::runtime_error{"Failed to create OpenGL context"};
            }
            return ctx;
        } (),
        Detail::ContextDeleter{dpy}
    } {}

    using Detail::ContextBase::get;
    EGLDisplay get_display() const {
        return get_deleter().get_display();
    }
};

class Surface: Detail::SurfaceBase {
    Surface(EGLDisplay dpy, EGLSurface surf):
        Detail::SurfaceBase{surf, Detail::SurfaceDeleter{dpy}} {}

public:
    static Surface makeXlibSurface(
        EGLDisplay dpy, EGLConfig cfg,
        Window win,
        const EGLAttrib* attrib_list = nullptr
    ) {
        auto surf = eglCreatePlatformWindowSurface(
            dpy, cfg, &win, attrib_list
        );
        if (!surf) {
            throw std::runtime_error{"Failed to create Xlib surface"};
        }
        return Surface{dpy, surf};
    }

    static Surface makeXCBSurface(
        EGLDisplay dpy, EGLConfig cfg,
        xcb_window_t win,
        const EGLAttrib* attrib_list = nullptr
    ) {
        // TODO:
        // casting win to void* is either a bug in mesa or
        // EGL_EXT_platform_xcb extension spec
        auto surf = eglCreatePlatformWindowSurface(
            dpy, cfg,
            reinterpret_cast<void *>(win),
            attrib_list
        );
        if (!surf) {
            throw std::runtime_error{"Failed to create XCB surface"};
        }
        return Surface{dpy, surf};
    }

    using Detail::SurfaceBase::get;
    EGLDisplay get_display() const {
        return get_deleter().get_display();
    }
};
}
