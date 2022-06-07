#include "GLInstance.hpp"
#include "GLContext.hpp"

#include <cassert>

namespace R1::GL {
namespace {
std::string displayRendererName(EGLDisplay dpy) {
    EGL::Context ctx{dpy};
    if (!eglMakeCurrent(dpy, EGL_NO_SURFACE, EGL_NO_SURFACE, ctx.get())) {
        throw std::runtime_error{"Failed to make temporary OpenGL context current"};
    }

    GladGLContext gl;
    if (!gladLoadGLContext(&gl, eglGetProcAddress)) {
        throw std::runtime_error{"Failed to load OpenGL function pointers"};
    }

    return reinterpret_cast<const char*>(gl.GetString(GL_RENDERER));
}

EGLConfig chooseSurfaceConfig(EGLDisplay dpy) {
    static constexpr std::array cfg_attrib_list {
        EGL_COLOR_BUFFER_TYPE, EGL_RGB_BUFFER,
        EGL_BLUE_SIZE, 8,
        EGL_GREEN_SIZE, 8,
        EGL_RED_SIZE, 8,
        EGL_RENDERABLE_TYPE, EGL_OPENGL_BIT,
        EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
        EGL_NONE,
    };

    EGLConfig cfg;
    EGLint cfg_cnt;
    if (!eglChooseConfig(dpy, cfg_attrib_list.data(), &cfg, 1, &cfg_cnt)) {
        throw std::runtime_error{"Failed to choose surface config"};
    }

    return cfg;
}
}

Instance::Instance(EGL::Display&& dpy, EGL::Surface&& surf):
    m_display{std::move(dpy)},
    m_surface{std::move(surf)},
    m_device_name{displayRendererName(m_display.get())} {}

Instance Instance::makeXlibInstance(Display* xdpy, Window win) {
    auto screen = [&] {
        XWindowAttributes attribs;
        if (!XGetWindowAttributes(xdpy, win, &attribs)) {
            throw std::runtime_error{"Failed to get screen of Xlib window"};
        }

        return XScreenNumberOfScreen(attribs.screen);
    } ();

    auto dpy = EGL::Display::makeXlibDisplay(xdpy, screen);

    auto cfg = chooseSurfaceConfig(dpy.get());
    static constexpr auto surf_attrib_list = make_array<EGLAttrib>(
        EGL_GL_COLORSPACE, EGL_GL_COLORSPACE_SRGB,
        EGL_NONE
    );
    auto surf = EGL::Surface::makeXlibSurface(dpy.get(), cfg, win, surf_attrib_list.data());

    return Instance{
        std::move(dpy), std::move(surf)
    };
}

Instance Instance::makeXCBInstance(xcb_connection_t* con, xcb_window_t win) {
    auto screen = [&] {
        auto geom = xcb_get_geometry_reply(
            con,
            xcb_get_geometry(con, win),
            nullptr
        );
        if (!geom) {
            throw std::runtime_error{"Failed to get root of XCB window"};
        }
        auto root_win = geom->root;
        free(geom);

        int screen = 0;
        for (auto iter = xcb_setup_roots_iterator(xcb_get_setup(con));
            iter.rem;
            xcb_screen_next(&iter), screen++) {
            if (iter.data->root == root_win) {
                return screen;
            }
        }

        throw std::runtime_error{"Failed to find screen of XCB window"};
    } ();

    auto dpy = EGL::Display::makeXCBDisplay(con, screen);

    auto cfg = chooseSurfaceConfig(dpy.get());
    static constexpr auto surf_attrib_list = make_array<EGLAttrib>(
        EGL_GL_COLORSPACE, EGL_GL_COLORSPACE_SRGB,
        EGL_NONE
    );
    auto surf = EGL::Surface::makeXCBSurface(dpy.get(), cfg, win, surf_attrib_list.data());

    return Instance{
        std::move(dpy), std::move(surf)
    };
}

size_t Instance::deviceCount() const {
    return 1;
}

size_t Instance::devices(std::span<DeviceID> devices) const {
    auto count = std::min(devices.size(), deviceCount());
    if (count) {
        devices[0] = c_dev;
    }
    return count;
}

const char* Instance::deviceName(DeviceID dev) const {
    assert(DeviceIDIsValid(dev));
    return m_device_name.c_str();
}

Context* Instance::createContext(DeviceID dev) {
    assert(DeviceIDIsValid(dev));
    return new Context(m_display.get(), m_surface.get());
}

bool Instance::DeviceIDIsValid(DeviceID dev) {
    return dev == c_dev;
}
}
