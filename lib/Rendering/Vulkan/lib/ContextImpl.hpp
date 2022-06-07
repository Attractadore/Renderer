#pragma once
#include "Context.hpp"
#include "VKRAII.hpp"

namespace R1::VK {
struct ContextImpl {
    Vk::Device device;
    Vk::Allocator allocator;
};
}
