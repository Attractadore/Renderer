#pragma once
#include "ContextCommon.hpp"
#include "Traits.hpp"

namespace R1::GAL {
Context CreateContext(Device dev, const ContextConfig& config);
void DestroyContext(Context ctx);

void ContextWaitIdle(Context ctx);
}
