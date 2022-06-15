#include "Context.hpp"

namespace R1::GAPI {
namespace {
GAL::Context CreateContext(Device& device) {
    auto graphics_queue = device.FindGraphicsQueueFamily();
    auto compute_queue = device.FindComputeQueueFamily();
    auto transfer_queue = device.FindTransferQueueFamily();

    using GAL::QueueFamily::ID::Unknown;

    unsigned config_cnt = 0;
    std::array<GAL::QueueConfig, 3> queue_configs;
    auto push_config = [&] (const GAL::QueueConfig& cfg) {
        queue_configs[config_cnt++] = cfg;
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

    // TODO: Should be optional, but currently
    // there is no support for headless rendering
    if (!device.GetDescription().wsi) {
        throw std::runtime_error{
            "Device doesn't support presentation"};
    }

    GAL::ContextConfig config {
        .queue_config = std::span{queue_configs.data(), config_cnt},
        .wsi = true,
    };

    return GAL::CreateContext(device.get(), config);
}
}

Context::Context(Device& device):
    m_device{device},
    m_context{CreateContext(device)}
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
