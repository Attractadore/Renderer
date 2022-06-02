#pragma once
#include "Types.hpp"

namespace R1::VK {
Context CreateContext(const ContextConfig& config, Device dev);
void DestroyContext(Context ctx);
}
