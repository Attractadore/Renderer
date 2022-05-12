#pragma once
#include "Types.hpp"

#include <concepts>
#include <span>

#include <X11/Xlib.h>
#include <xcb/xcb.h>

namespace R1 {
template<typename T>
concept ISwapchain = requires() {
    true;
};

template<typename P>
concept ISwapchainPointer =
    ISwapchain<std::remove_pointer_t<P>>;

template<typename T>
concept IContext = requires(
    T c, const T cc,
    std::span<PresentMode> pmode_out,
    SizeCallback&& size_cb, PresentMode pmode
) { 
    { cc.presentModeCount() }                           -> std::unsigned_integral;
    { cc.presentModes(pmode_out) }                      -> std::unsigned_integral;

    { c.createSwapchain(std::move(size_cb), pmode) }    -> ISwapchainPointer;
};

template<typename P>
concept IContextPointer =
    IContext<std::remove_pointer_t<P>>;

template<typename T>
concept IInstance = requires(
    T i, const T ci,
    ::Display* xlib_dpy, ::Window xlib_win,
    xcb_connection_t* xcb_con, xcb_window_t xcb_win,
    std::span<DeviceID> devs_out,
    DeviceID dev
) {
    T(xlib_dpy, xlib_win);
    T(xcb_con, xcb_win);

    { ci.deviceCount() }     -> std::unsigned_integral;
    { ci.deviceCount() }     -> std::unsigned_integral;
    { ci.devices(devs_out) } -> std::unsigned_integral;
    { ci.deviceName(dev) }   -> std::same_as<const char*>;

    { i.createContext(dev) } -> IContextPointer;
};
}
