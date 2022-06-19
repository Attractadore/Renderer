#include "GALListDevicesBase.hpp"
#include "GAL/GALWayland.hpp"

int main(int argc, char *argv[]) {
    RunFromSysWM([] (const SDL_SysWMinfo& info, const R1::GAL::InstanceConfig& config) {
        R1::GAL::Instance instance = nullptr;
        if (info.subsystem == SDL_SYSWM_WAYLAND) {
            instance = R1::GAL::Wayland::CreateInstance(info.info.wl.display, config);
        }
        return instance;
    }, "Created Wayland instance", "Not running on Wayland!");
};
