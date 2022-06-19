#include "GALListDevicesBase.hpp"
#include "GAL/GALWin32.hpp"

int main(int argc, char *argv[]) {
    RunFromSysWM([] (const SDL_SysWMinfo& info, const R1::GAL::InstanceConfig& config) {
        R1::GAL::Instance instance = nullptr;
        if (info.subsystem == SDL_SYSWM_WINDOWS) {
            instance = R1::GAL::Win32::CreateInstance(info.info.win.hdc, config);
        }
        return instance;
    }, "Created Win32 instance", "Not running on Win32!");
};
