#include "Pipeline.hpp"
#include "VKUtil.hpp"

namespace R1::VK {
ShaderModule CreateShaderModule(Context ctx, const ShaderModuleConfig& config) {
    const auto& code = config.code;
    VkShaderModule module = VK_NULL_HANDLE;
    VkShaderModuleCreateInfo create_info = {
        .sType = sType(create_info),
        .codeSize = code.size(),
        .pCode = reinterpret_cast<const uint32_t*>(code.data()),
    };
    vkCreateShaderModule(ctx->device.get(), &create_info, nullptr, &module);
    if (!module) {
        throw std::runtime_error{"Vulkan: Failed to create shader module"};
    }
    return module;
}
}