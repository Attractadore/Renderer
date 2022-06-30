#pragma once
#include "VulkanInstance.hpp"
#include "DeviceImpl.hpp"
#include "VKRAII.hpp"

#include <span>

namespace R1::GAL {
struct InstanceImpl {
    Vk::Instance instance;
    std::vector<DeviceImpl> devices;
};
}
