#include "DrawTriangleBase.hpp"
#include "R1/R1Win32.h"

int main() {
    return RunFromSysWMInfo(
        [] (const SDL_SysWMinfo& info) {
            std::cerr << "Create Win32 instance\n";
            if (info.subsystem != SDL_SYSWM_WINDOWS) {
                std::cerr << "Not running on Win32!\n";
                return static_cast<R1Instance*>(nullptr);
            }
            return R1_CreateInstanceWin32(info.info.win.hdc);
        },
        [] (R1Instance* instance, const SDL_SysWMinfo& info, SDL_Window* window) {
            std::cerr << "Create Win32 surface\n";
            if (info.subsystem != SDL_SYSWM_WINDOWS) {
                std::cerr << "Not running on Win32!\n";
                return static_cast<R1Surface*>(nullptr);
            }
            return R1_CreateSurfaceWin32(
                instance,
                info.info.win.window,
                reinterpret_cast<R1SurfaceSizeCallback>(SDL_GetWindowSize),
                window);
        });
}
