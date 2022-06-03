#pragma once
#include "Pipeline.hpp"

namespace R1::VK {
VkColorComponentFlags ColorComponentMaskToVK(const ColorAttachmentInfo::ComponentMask& mask);
}

#include "PipelineImpl.inl"
