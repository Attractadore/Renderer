#pragma once
#include "Context.hpp"

namespace R1 {
class Scene {
    Context* m_context;

public:
    Scene(Context* ctx);

    void draw();
};
};
