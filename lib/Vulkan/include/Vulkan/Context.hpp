#pragma once
#include "Common/Context.hpp"
#include "Device.hpp"
#include "VKRAII.hpp"

namespace R1::VK {
struct ContextImpl {
    Vk::Device device;
};
using Context = ContextImpl*;

Context CreateContext(Device dev, const ContextConfig& config);
void DestroyContext(Context ctx);
}
