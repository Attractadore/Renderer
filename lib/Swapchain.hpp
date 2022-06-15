#pragma once
#include "R1Types.h"
#include "GAPI/Swapchain.hpp"

namespace R1 {
namespace GAPI { class Context; }

class Surface {
    GAPI::Surface m_surface;

public:
    explicit Surface(GAPI::Surface surface):
        m_surface{std::move(surface)} {}

    GAPI::Surface& get() noexcept { return m_surface; }
};

class Swapchain {
    GAPI::Swapchain m_swapchain;

public:
    Swapchain(GAPI::Context& ctx, GAPI::Surface& surface);

    GAPI::Swapchain& get() noexcept { return m_swapchain; }
};
}

#include "Swapchain.inl"
