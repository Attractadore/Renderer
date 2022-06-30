#pragma once
#include "GAPI/Instance.hpp"

class R1Instance {
protected:
    R1::GAPI::Instance m_instance;

public:
    explicit R1Instance(R1::GAPI::Instance instance):
        m_instance{std::move(instance)} {}

    size_t GetDeviceCount() const {
        return m_instance.GetDeviceCount();
    }

    R1::GAPI::Device& GetDevice(size_t idx) {
        return m_instance.GetDevice(idx);
    }

    R1::GAPI::Instance& get() noexcept { return m_instance; }
};

namespace R1 {
using Instance = R1Instance;
}
