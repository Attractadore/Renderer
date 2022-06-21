#pragma once
#include "Common/RAII.hpp"

#include <EGL/egl.h>

namespace R1::GAL::egl {
namespace Detail {
template<class H>
constexpr int DoDisplayDeleteF = 0;

struct DisplayChildHandleDeleter {
    template<class H>
    static void DoDelete(EGLDisplay dpy, H handle) noexcept {
        DoDisplayDeleteF<H>(dpy, handle);
    }
};

template<class H>
using DisplayHandleBase = ChildHandle<EGLDisplay, H, DisplayChildHandleDeleter>;
template<class H>
struct DisplayHandle: DisplayHandleBase<H>{
    using DisplayHandleBase<H>::DisplayHandleBase;

    EGLDisplay get_display() const noexcept {
        return this->get_deleter().get_parent();
    }
};

struct DisplayDeleter {
    void operator()(EGLDisplay dpy) const { eglTerminate(dpy); }
};
}

template<> inline constexpr auto Detail::DoDisplayDeleteF<EGLContext> = eglDestroyContext;
using Context = Detail::DisplayHandle<EGLContext>;
}
