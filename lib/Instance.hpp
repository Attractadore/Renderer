#pragma once
#include "GAPI/Instance.hpp"

namespace R1 {
class Instance {
    GAPI::Instance m_instance;

public:
    explicit Instance(GAPI::Instance instance):
        m_instance{std::move(instance)} {}

    size_t GetDeviceCount() const {
        return m_instance.GetDeviceCount();
    }

    GAPI::Device& GetDevice(size_t idx) {
        return m_instance.GetDevice(idx);
    }

    GAPI::Instance& get() noexcept { return m_instance; }
};
}
