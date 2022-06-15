#pragma once
#include "Common/Flags.hpp"

#if GAL_USE_VULKAN
#include "VulkanBaseTypes.hpp"
#endif

namespace R1::GAL {
using ColorComponentFlags = Flags<ColorComponent>;
using CommandBufferUsageFlags = Flags<CommandBufferUsage>;
using CommandPoolConfigFlags = Flags<CommandPoolConfigOption>;
using ImageAspectFlags = Flags<ImageAspect>;
using ImageConfigFlags = Flags<ImageConfigOption>;
using ImageUsageFlags = Flags<ImageUsage>;
using MemoryAccessFlags = Flags<MemoryAccess>;
using PipelineStageFlags = Flags<PipelineStage>;
using QueueCapabilityFlags = Flags<QueueCapability>;
using RenderingConfigFlags = Flags<RenderingConfigOption>;
}
