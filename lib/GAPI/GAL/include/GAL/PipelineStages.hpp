#pragma once
#if GAL_USE_VULKAN
#include "VulkanPipelineStages.hpp"
#endif

#include "Common/Flags.hpp"

namespace R1::GAL {
namespace Detail {
template<typename E>
concept IsShaderStage = requires {
    E::Vertex;
    E::TessellationControl;
    E::TessellationEvaluation;
    E::Geometry;
    E::Fragment;
    E::Compute;
    E::All;
};

template<typename E>
concept IsPipelineStage = requires {
    E::DrawIndirect;

    E::IndexInput;
    E::VertexAttributeInput;
    E::VertexInput;

    E::VertexShader;
    E::TesselationControlShader;
    E::TesselationEvaluationShader;
    E::GeometryShader;
    E::PreRasterizationShaders;

    E::FragmentShader;
    E::EarlyFragmentTests;
    E::LateFragmentTests;
    E::ColorAttachmentOutput;

    E::AllGraphics;

    E::ComputeShader;
    E::AllCompute;

    E::Copy;
    E::Resolve;
    E::Blit;
    E::Clear;
    E::AllTransfer;

    E::AllCommands;
};

template<typename E>
concept IsMemoryAccess = requires {
    E::IndirectCommandRead;

    E::IndexRead;
    E::VertexAttributeRead;
    E::VertexRead;

    E::ShaderUniformRead;
    E::ShaderSampledRead;
    E::ShaderStorageRead;
    E::ShaderRead;

    E::ShaderStorageWrite;
    E::ShaderWrite;

    E::ColorAttachmentRead;
    E::DepthAttachmentRead;
    E::StencilAttachmentRead;
    E::DepthStencilAttachmentRead;
    E::AttachmentRead;

    E::ColorAttachmentWrite;
    E::DepthAttachmentWrite;
    E::StencilAttachmentWrite;
    E::DepthStencilAttachmentWrite;
    E::AttachmentWrite;

    E::TransferRead;
    E::TransferWrite;

    E::MemoryRead;
    E::MemoryWrite;
};

static_assert(IsShaderStage<ShaderStage>);
static_assert(IsPipelineStage<PipelineStage>);
static_assert(IsMemoryAccess<MemoryAccess>);
}

using ShaderStageFlags      = Flags<ShaderStage>;
using PipelineStageFlags    = Flags<PipelineStage>;
using MemoryAccessFlags     = Flags<MemoryAccess>;
}
