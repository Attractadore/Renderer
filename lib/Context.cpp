#include "Context.hpp"

namespace R1 {
namespace {
QueueFamily::ID FindGraphicsQueueFamily(const Rendering::Device& device) {
    const auto& desc = device.GetDescription();
    for (const auto& qf: desc.queue_families) {
        if (qf.capabilities.graphics) {
            return qf.id;
        }

    }
    throw std::runtime_error{"Device doesn't have a graphics queue family"};
}

Rendering::Context CreateContext(Rendering::Device& device) {
    QueueConfig qcfg = {
        .id = FindGraphicsQueueFamily(device),
        .count = 1,
    };
    ContextConfig config {
        .queue_config = std::span{&qcfg, 1},
        // TODO: should be optional
        .wsi = true,
    };
    return Rendering::Context{device, config};
}
}

Context::Context(Rendering::Device& device):
    m_context{CreateContext(device)} {}
}
