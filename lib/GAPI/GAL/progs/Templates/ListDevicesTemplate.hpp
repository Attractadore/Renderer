#include "GAL/GAL.hpp"

#include <iostream>

inline const char* GetDeviceTypeStr(R1::GAL::DeviceType dev_type) {
    using enum R1::GAL::DeviceType;
    switch(dev_type) {
        case CPU:
            return "CPU";
        case IntegratedGPU:
            return "iGPU";
        case DiscreteGPU:
            return "dGPU";
        case VirtualGPU:
            return "virtual GPU";
        default:
            return "unknown";
    }
}

inline void printDevices(R1::GAL::Instance instance) {
    auto dev_cnt = R1::GAL::GetDeviceCount(instance);
    for (size_t i = 0; i < dev_cnt; i++) {
        auto dev = R1::GAL::GetDevice(instance, i);
        const auto& dev_desc = R1::GAL::GetDeviceDescription(dev);
        std::cout << GetDeviceTypeStr(dev_desc.type) << " device " << dev_desc.name << "\n";
    }
}

template<typename F>
void ListDevicesTemplate(
    F&& create_instance,
    const char* success, const char* error
) {
    auto i = create_instance();
    if (i) {
        std::cout << success << "\n";
        printDevices(i);
    } else {
        std::cout << error << "\n";
    }
    R1::GAL::DestroyInstance(i);
};
