#include "CreateContextsTemplate.hpp"

namespace {
using namespace R1::GAL;
Instance CreateInstance() {
    VkInstanceCreateInfo create_info = {
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
    };
    return Vulkan::CreateInstanceFromTemplate(
        vkGetInstanceProcAddr, &create_info);
}

Context CreateContext(Device adapter, const ContextConfig& config) {
    VkDeviceCreateInfo create_info = {
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
    };
    return Vulkan::CreateContextFromTemplate(
        adapter, config, &create_info);
}
}

int main() {
    CreateContextsTemplate(
        CreateInstance, CreateContext,
        "Created Vulkan instance",
        "Failed to create Vulkan instance");
}
