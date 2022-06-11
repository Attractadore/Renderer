#pragma once
#include "GAPIRAII.hpp"
#include "Instance.hpp"

namespace R1::Rendering {
class Device;

class Context {
    Device&         m_device;
    HContext        m_context;
    QueueFamily::ID m_graphics_queue_family;
    QueueFamily::ID m_compute_queue_family;
    QueueFamily::ID m_transfer_queue_family;
    GAPI::Queue     m_graphics_queue;
    GAPI::Queue     m_compute_queue;
    GAPI::Queue     m_transfer_queue;

public:
    Context(Device& device);

    Device& GetDevice() noexcept { return m_device; }
    GAPI::Context get() noexcept { return m_context.get(); }

    QueueFamily::ID GetGraphicsQueueFamily() noexcept { return m_graphics_queue_family; }
    QueueFamily::ID GetComputeQueueFamily() noexcept { return m_compute_queue_family; }
    QueueFamily::ID GetTransferQueueFamily() noexcept { return m_transfer_queue_family; }
    GAPI::Queue GetGraphicsQueue() noexcept { return m_graphics_queue; }
    GAPI::Queue GetComputeQueue() noexcept { return m_compute_queue; }
    GAPI::Queue GetTransferQueue() noexcept { return m_transfer_queue; }
};
}
