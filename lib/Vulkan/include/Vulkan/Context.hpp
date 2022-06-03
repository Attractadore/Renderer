#pragma once
#include "Common/Context.hpp"
#include "Device.hpp"

namespace R1::VK {
struct ContextImpl;
using Context = ContextImpl*;

Context CreateContext(Device dev, const ContextConfig& config);
void DestroyContext(Context ctx);
}
