#pragma once
#include "GAPI/Instance.hpp"
#include "R1Types.h"

namespace R1 {
inline R1Device* ToPublic(R1::GAPI::Device* device) {
    return reinterpret_cast<R1Device*>(device);
}

inline R1::GAPI::Device* ToPrivate(R1Device* device) {
    return reinterpret_cast<R1::GAPI::Device*>(device);
}
}
