#pragma once
#include "Common/Interfaces.hpp"
#include "EGLRAII.hpp"
#include "GLTypes.hpp"

#include <span>

namespace R1::GL {
class Instance {
    EGL::Display m_display;
    EGL::Surface m_surface;
    std::string m_device_name;

    static constexpr DeviceID c_dev = 1;

private:
    Instance(EGL::Display&& dpy, EGL::Surface&& surf);

public:
    static Instance makeXlibInstance(Display* dpy, Window win);
    static Instance makeXCBInstance(xcb_connection_t* con, xcb_window_t win);

    size_t deviceCount() const;
    size_t devices(std::span<DeviceID> out) const;
    const char* deviceName(DeviceID dev) const;

    Context* createContext(DeviceID dev);

private:
    static bool DeviceIDIsValid(DeviceID dev);
};
static_assert(IInstance<Instance>);
}
