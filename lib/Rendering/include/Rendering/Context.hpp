#pragma once
#include "GAPIRAII.hpp"
#include "Instance.hpp"

namespace R1::Rendering {
class Device;

class Context {
    Device&         m_device;
    HContext        m_context;
    GAPI::Queue     m_graphics_queue;
    GAPI::Queue     m_compute_queue;
    GAPI::Queue     m_transfer_queue;

public:
    Context(Device& device);

    Device& GetDevice() noexcept { return m_device; }
    GAPI::Context get() noexcept { return m_context.get(); }

    GAPI::Queue GetGraphicsQueue() noexcept { return m_graphics_queue; }
    GAPI::Queue GetComputeQueue() noexcept { return m_compute_queue; }
    GAPI::Queue GetTransferQueue() noexcept { return m_transfer_queue; }
};
}
