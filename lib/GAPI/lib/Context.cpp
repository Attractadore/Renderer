#include "ContextImpl.hpp"

namespace R1::GAPI {
ContextConfig ConfigureContext(Device& device) {
    ContextConfig cfg;

    auto graphics_queue = device.FindGraphicsQueueFamily();
    auto compute_queue = device.FindComputeQueueFamily();
    auto transfer_queue = device.FindTransferQueueFamily();

    using GAL::QueueFamily::ID::Unknown;

    auto& queue_configs = cfg.queue_configs;
    auto push_config = [&] (const GAL::QueueConfig& cfg) {
        queue_configs.push_back(cfg);
    };
    auto push_config_if_found = [&] (const GAL::QueueConfig& cfg) {
        if (cfg.id != Unknown) {
            push_config(cfg);
        }
    };

    if (graphics_queue == Unknown) {
        throw std::runtime_error{
            "Device doesn't have a graphics queue family"};
    }
    push_config({
        .id = graphics_queue,
        .count = 1,
    });
    push_config_if_found({
        .id = compute_queue,
        .count = 1,
    });
    push_config_if_found({
        .id = transfer_queue,
        .count = 1,
    });

    cfg.config = {
        .queue_config = queue_configs,
    };
    
    return cfg;
}

Context::Context(Device& device, HContext ctx):
    m_device{device},
    m_context{std::move(ctx)}
{
    using GAL::QueueFamily::ID::Unknown;
    m_graphics_queue_family = device.FindGraphicsQueueFamily();
    m_compute_queue_family = device.FindComputeQueueFamily();
    if (m_compute_queue_family == Unknown) {
        m_compute_queue_family = m_graphics_queue_family;
    }
    m_transfer_queue_family = device.FindTransferQueueFamily();
    if (m_transfer_queue_family == Unknown) {
        m_transfer_queue_family = m_graphics_queue_family;
    }

    m_graphics_queue = GAL::GetQueue(m_context.get(), m_graphics_queue_family, 0);
    m_compute_queue = GAL::GetQueue(m_context.get(), m_compute_queue_family, 0);
    m_transfer_queue = GAL::GetQueue(m_context.get(), m_transfer_queue_family, 0);
}
}
