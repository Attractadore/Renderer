#pragma once

namespace R1::VK {
inline VkColorComponentFlags ColorComponentMaskToVK(
    const ColorAttachmentInfo::ComponentMask& mask
) {
    VkColorComponentFlags flags = 0;
    flags |= mask.r ? VK_COLOR_COMPONENT_R_BIT: 0;
    flags |= mask.g ? VK_COLOR_COMPONENT_G_BIT: 0;
    flags |= mask.b ? VK_COLOR_COMPONENT_B_BIT: 0;
    flags |= mask.a ? VK_COLOR_COMPONENT_A_BIT: 0;
    return flags;
}
}
