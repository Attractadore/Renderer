#pragma once
#include "GAPI/Context.hpp"

namespace R1::GAPI::Vulkan {
Context CreateContextFromTemplate(Device& device, const VkDeviceCreateInfo* create_template);
}
