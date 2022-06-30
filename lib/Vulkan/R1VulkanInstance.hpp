#pragma once
#include "GAPI/Vulkan/Instance.hpp"
#include "Instance.hpp"

namespace R1 {
class VulkanInstance: public Instance {
public:
    explicit VulkanInstance(
        PFN_vkGetInstanceProcAddr loader,
        const VkInstanceCreateInfo* create_template
    ):
        Instance{GAPI::Vulkan::CreateInstanceFromTemplate(loader, create_template)} {}

    VkInstance GetVkInstance() noexcept {
        return GAL::Vulkan::GetVkInstance(m_instance.get());
    }
};
}
