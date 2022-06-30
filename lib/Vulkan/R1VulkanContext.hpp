#pragma once
#include "GAPI/Vulkan/Context.hpp"
#include "Context.hpp"

namespace R1 {
class VulkanContext: public Context {
public:
    VulkanContext(
        GAPI::Device& device,
        const VkDeviceCreateInfo* create_template
    ):
        Context{GAPI::Vulkan::CreateContextFromTemplate(
            device, create_template)} {}

    VkDevice GetVkDevice() const noexcept {
        return GAL::Vulkan::GetVkDevice(m_context.get());
    }
};
}
