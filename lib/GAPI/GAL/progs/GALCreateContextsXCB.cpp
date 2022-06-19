#include "GALCreateContextsBase.hpp"
#include "GAL/GALXCB.hpp"

#include <X11/Xlib-xcb.h>

int main(int argc, char *argv[]) {
    RunFromSysWM([] (const SDL_SysWMinfo& info, const R1::GAL::InstanceConfig& config) {
        R1::GAL::Instance instance = nullptr;
        if (info.subsystem == SDL_SYSWM_X11) {
            instance = R1::GAL::XCB::CreateInstance(XGetXCBConnection(info.info.x11.display), 0, config);
        }
        return instance;
    }, "Created XCB instance", "Not running on X11!");
};
