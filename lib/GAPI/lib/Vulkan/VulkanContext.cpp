#include "GAPI/Vulkan/Context.hpp"
#include "ContextImpl.hpp"

namespace R1::GAPI::Vulkan {
Context CreateContextFromTemplate(
    Device& device,
    const VkDeviceCreateInfo* create_template
) {
    auto cfg = ConfigureContext(device);
    return Context{
        device,
        HContext{GAL::Vulkan::CreateContextFromTemplate(
            device.get(), cfg.config, create_template)}};
}
}
