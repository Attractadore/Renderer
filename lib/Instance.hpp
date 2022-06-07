#pragma once
#include "Rendering/Instance.hpp"

namespace R1 {
class Instance {
    Rendering::Instance m_instance;

public:
    explicit Instance(Rendering::Instance instance):
        m_instance{std::move(instance)} {}

    size_t GetDeviceCount() const {
        return m_instance.GetDeviceCount();
    }

    Rendering::Device& GetDevice(size_t idx) {
        return m_instance.GetDevice(idx);
    }

    Rendering::Instance& get() noexcept { return m_instance; }
};
}
