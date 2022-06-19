#include "DrawTriangleBase.hpp"
#include "R1/R1XCB.h"

#include <X11/Xlib-xcb.h>

int main() {
    return RunFromSysWMInfo(
        [] (const SDL_SysWMinfo& info) {
            std::cerr << "Create X11 instance\n";
            if (info.subsystem != SDL_SYSWM_X11) {
                std::cerr << "Not running on X11!\n";
                return static_cast<R1Instance*>(nullptr);
            }
            return R1_CreateInstanceXCB(XGetXCBConnection(info.info.x11.display), 0);
        },
        [] (R1Instance* instance, const SDL_SysWMinfo& info, SDL_Window* window) {
            std::cerr << "Create X11 surface\n";
            if (info.subsystem != SDL_SYSWM_X11) {
                std::cerr << "Not running on X11!\n";
                return static_cast<R1Surface*>(nullptr);
            }
            return R1_CreateSurfaceXCB(
                instance,
                XGetXCBConnection(info.info.x11.display), info.info.x11.window,
                reinterpret_cast<R1SurfaceSizeCallback>(SDL_GetWindowSize),
                window);
        });
}
