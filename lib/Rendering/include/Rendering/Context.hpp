#pragma once
#include "GAPIRAII.hpp"
#include "Instance.hpp"

namespace R1::Rendering {
class Device;

class Context {
    HContext m_context;

public:
    Context(Device& device, const ContextConfig& config):
        m_context{GAPI::CreateContext(device.get(), config)} {}

    GAPI::Context get() noexcept { return m_context.get(); }
};
}
