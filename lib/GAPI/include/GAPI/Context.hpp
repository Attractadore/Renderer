#pragma once
#include "GALRAII.hpp"
#include "Instance.hpp"

namespace R1::GAPI {
class Device;

class Context {
    Device&                 m_device;
    HContext                m_context;
    GAL::QueueFamily::ID    m_graphics_queue_family;
    GAL::QueueFamily::ID    m_compute_queue_family;
    GAL::QueueFamily::ID    m_transfer_queue_family;
    GAL::Queue              m_graphics_queue;
    GAL::Queue              m_compute_queue;
    GAL::Queue              m_transfer_queue;

public:
    Context(Device& device, HContext ctx);

    Device& GetDevice() noexcept { return m_device; }
    GAL::Context get() const noexcept { return m_context.get(); }

    GAL::QueueFamily::ID GetGraphicsQueueFamily() noexcept { return m_graphics_queue_family; }
    GAL::QueueFamily::ID GetComputeQueueFamily() noexcept { return m_compute_queue_family; }
    GAL::QueueFamily::ID GetTransferQueueFamily() noexcept { return m_transfer_queue_family; }
    GAL::Queue GetGraphicsQueue() noexcept { return m_graphics_queue; }
    GAL::Queue GetComputeQueue() noexcept { return m_compute_queue; }
    GAL::Queue GetTransferQueue() noexcept { return m_transfer_queue; }
};
}
