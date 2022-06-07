#pragma once
#include "Rendering/Context.hpp"

namespace R1 {
class Context {
    Rendering::Context m_context;

public:
    explicit Context(Rendering::Device& device);

    Rendering::Context& get() noexcept { return m_context; }
};
}
