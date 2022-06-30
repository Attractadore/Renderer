#pragma once
#include <vulkan/vulkan.h>

namespace R1::GAL {
struct ContextImpl;
using Context = ContextImpl*;

namespace Vulkan {
VkDevice GetVkDevice(Context ctx);
}
}
