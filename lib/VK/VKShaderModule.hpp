#pragma once
#include "Common/Ref.hpp"
#include "VKRAII.hpp"

#include <span>

namespace R1::VK {
struct ShaderModuleConfig {
    std::span<const std::byte> code;
};

struct ShaderModule: RefedBase<ShaderModule> {
    Vk::ShaderModule m_module;

protected:
    ShaderModule(Vk::ShaderModule module):
        m_module{std::move(module)} {}
};
using ShaderModuleRef = Ref<ShaderModule>;
};
