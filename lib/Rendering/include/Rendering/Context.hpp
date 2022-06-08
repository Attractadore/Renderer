#pragma once
#include "GAPIRAII.hpp"
#include "Instance.hpp"

namespace R1::Rendering {
class Device;

class Context {
    Device&         m_device;
    HContext        m_context;
    QueueFamily::ID m_graphics_queue_family;
    GAPI::Queue     m_graphics_queue;

public:
    Context(Device& device, const ContextConfig& config):
        m_device{device},
        m_context{GAPI::CreateContext(device.get(), config)}
    {
        // TODO: this is obviously temporary
        assert(config.queue_config.size() == 1);
        m_graphics_queue_family = config.queue_config[0].id;
        m_graphics_queue = GAPI::GetQueue(m_context.get(), m_graphics_queue_family, 0);
    }

    GAPI::Context get() noexcept { return m_context.get(); }
    Device& GetDevice() noexcept { return m_device; }
    GAPI::Queue GetGraphicsQueue() noexcept { return m_graphics_queue; }
};
}
