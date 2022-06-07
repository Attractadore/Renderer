#pragma once
#include "R1Types.h"
#include "Rendering/Swapchain.hpp"

namespace R1 {
namespace Rendering { class Context; }

class Surface {
    Rendering::Surface m_surface;

public:
    explicit Surface(Rendering::Surface surface):
        m_surface{std::move(surface)} {}

    Rendering::Surface& get() noexcept { return m_surface; }
};

class Swapchain {
    Rendering::Swapchain m_swapchain;

public:
    Swapchain(Rendering::Context& ctx, Rendering::Surface& surface);

    Rendering::Swapchain& get() noexcept { return m_swapchain; }
};
}

#include "Swapchain.inl"
