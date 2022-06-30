#pragma once
#include "GAL/Context.hpp"

namespace R1::GAL {
namespace Vulkan {
Context CreateContextFromTemplate(Device dev, const ContextConfig& config, const VkDeviceCreateInfo* create_template);
}
}
