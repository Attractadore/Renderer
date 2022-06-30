#pragma once
#include "GAPI/GALRAII.hpp"
#include "GAPI/Instance.hpp"

namespace R1::GAPI::Vulkan {
inline Instance CreateInstanceFromTemplate(
    PFN_vkGetInstanceProcAddr loader,
    const VkInstanceCreateInfo* create_template
) {
    HInstance instance{GAL::Vulkan::CreateInstanceFromTemplate(loader, create_template)};
    return Instance{std::move(instance)};
}
}
