#include "DrawTriangleBase.hpp"
#include "R1/R1Wayland.h"

int main() {
    return RunFromSysWMInfo(
        [] (const SDL_SysWMinfo& info) {
            std::cerr << "Create Wayland instance\n";
            if (info.subsystem != SDL_SYSWM_WAYLAND) {
                std::cerr << "Not running on Wayland!\n";
                return static_cast<R1Instance*>(nullptr);
            }
            return R1_CreateInstanceWayland(info.info.wl.display);
        },
        [] (R1Instance* instance, const SDL_SysWMinfo& info, SDL_Window* window) {
            std::cerr << "Create Wayland surface\n";
            if (info.subsystem != SDL_SYSWM_WAYLAND) {
                std::cerr << "Not running on Wayland!\n";
                return static_cast<R1Surface*>(nullptr);
            }
            return R1_CreateSurfaceWayland(
                instance,
                info.info.wl.display, info.info.wl.surface,
                reinterpret_cast<R1SurfaceSizeCallback>(SDL_GetWindowSize),
                window);
        });
}
