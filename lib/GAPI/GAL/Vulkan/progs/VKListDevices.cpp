#include "ListDevicesTemplate.hpp"

namespace {
using namespace R1::GAL;
Instance CreateInstance() {
    VkInstanceCreateInfo create_info = {
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
    };
    return Vulkan::CreateInstanceFromTemplate(
        vkGetInstanceProcAddr, &create_info);
}
}

int main() {
    ListDevicesTemplate(
        CreateInstance,
        "Created Vulkan instance",
        "Failed to create Vulkan instance");
}
