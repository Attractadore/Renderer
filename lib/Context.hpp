#pragma once
#include "GAPI/Context.hpp"

namespace R1 {
class Context {
    GAPI::Context m_context;

public:
    explicit Context(GAPI::Device& device):
        m_context{device} {}

    GAPI::Context& get() noexcept { return m_context; }
};
}
