#pragma once
#include "Types.hpp"

namespace R1::VK {
ShaderModule CreateShaderModule(Context ctx, const ShaderModuleConfig& config);
void DestroyShaderModule(Context ctx, ShaderModule module);
}
