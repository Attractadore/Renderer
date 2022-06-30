#pragma once
#include <vulkan/vulkan.h>

namespace R1::GAL {
struct InstanceImpl;
using Instance = InstanceImpl*;

namespace Vulkan {
Instance CreateInstanceFromTemplate(
    PFN_vkGetInstanceProcAddr loader,
    const VkInstanceCreateInfo* create_template
);

VkInstance GetVkInstance(Instance instance);
}
}
