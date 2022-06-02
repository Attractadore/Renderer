#pragma once
#include "GAPI.hpp"

namespace R1 {
class Scene {
    GAPI::Context* m_context;

public:
    Scene(GAPI::Context* ctx);

    void draw();
};
};
