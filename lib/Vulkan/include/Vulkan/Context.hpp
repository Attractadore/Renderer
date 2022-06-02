#pragma once
#include "Types.hpp"

namespace R1::VK {
Context CreateContext(Device dev, const ContextConfig& config);
void DestroyContext(Context ctx);
}
