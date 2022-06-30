#include "GAL/GAL.hpp"

#include <iostream>

template<typename CreateContext>
void printDevices(R1::GAL::Instance instance, CreateContext&& create_context) {
    auto dev_cnt = R1::GAL::GetDeviceCount(instance);
    for (size_t i = 0; i < dev_cnt; i++) {
        auto dev = R1::GAL::GetDevice(instance, i);
        const auto& dev_desc = R1::GAL::GetDeviceDescription(dev);
        std::cout << "Create context for device " << dev_desc.name << "\n";
        if (dev_desc.queue_families.empty()) {
            std::cout << "Device doesn't have any queues\n";
            continue;
        }
        R1::GAL::QueueConfig qcfg = {
            .id = dev_desc.queue_families.front().id,
            .count = 1,
        };
        auto ctx = create_context(dev, {
            .queue_config = {&qcfg, 1},
        });
        if (!ctx) {
            std::cout << "Failed to create context for device\n";
        } else {
            std::cout << "Success\n";
        }
        R1::GAL::DestroyContext(ctx);
    }
}

template<typename CreateInstance, typename CreateContext>
void CreateContextsTemplate(
    CreateInstance&& create_instance, CreateContext&& create_context,
    const char* success, const char* error
) {
    auto i = create_instance();
    if (i) {
        std::cout << success << "\n";
        printDevices(i, std::forward<CreateContext>(create_context));
    } else {
        std::cout << error << "\n";
    }
    R1::GAL::DestroyInstance(i);
};
