#pragma once
#include "GAPI/Context.hpp"

class R1Context {
protected:
    R1::GAPI::Context m_context;

public:
    explicit R1Context(R1::GAPI::Context context):
        m_context{std::move(context)} {}

    R1::GAPI::Context& get() noexcept { return m_context; }
};

namespace R1 {
using Context = R1Context;
}
