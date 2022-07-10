#pragma once
#include <vulkan/vulkan.h>

namespace R1::GAL {
enum class ShaderStage {
    Vertex                  = VK_SHADER_STAGE_VERTEX_BIT,
    TessellationControl     = VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT,
    TessellationEvaluation  = VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT,
    Geometry                = VK_SHADER_STAGE_GEOMETRY_BIT,
    Fragment                = VK_SHADER_STAGE_FRAGMENT_BIT,
    Compute                 = VK_SHADER_STAGE_COMPUTE_BIT,
    All                     = VK_SHADER_STAGE_ALL_GRAPHICS,
};

enum class PipelineStage: VkPipelineStageFlags2 {
    DrawIndirect                = VK_PIPELINE_STAGE_2_DRAW_INDIRECT_BIT,

    IndexInput                  = VK_PIPELINE_STAGE_2_INDEX_INPUT_BIT,
    VertexAttributeInput        = VK_PIPELINE_STAGE_2_VERTEX_ATTRIBUTE_INPUT_BIT,
    VertexInput                 = VK_PIPELINE_STAGE_2_VERTEX_INPUT_BIT,

    VertexShader                = VK_PIPELINE_STAGE_2_VERTEX_SHADER_BIT,
    TesselationControlShader    = VK_PIPELINE_STAGE_2_TESSELLATION_CONTROL_SHADER_BIT,
    TesselationEvaluationShader = VK_PIPELINE_STAGE_2_TESSELLATION_EVALUATION_SHADER_BIT,
    GeometryShader              = VK_PIPELINE_STAGE_2_GEOMETRY_SHADER_BIT,
    PreRasterizationShaders     = VK_PIPELINE_STAGE_2_PRE_RASTERIZATION_SHADERS_BIT,

    FragmentShader              = VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT,
    EarlyFragmentTests          = VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT,
    LateFragmentTests           = VK_PIPELINE_STAGE_2_LATE_FRAGMENT_TESTS_BIT,
    ColorAttachmentOutput       = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,

    AllGraphics                 = VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT,

    ComputeShader               = VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT,
    AllCompute                  = ComputeShader,

    Copy                        = VK_PIPELINE_STAGE_2_COPY_BIT,
    Resolve                     = VK_PIPELINE_STAGE_2_RESOLVE_BIT,
    Blit                        = VK_PIPELINE_STAGE_2_BLIT_BIT,
    Clear                       = VK_PIPELINE_STAGE_2_CLEAR_BIT,
    AllTransfer                 = VK_PIPELINE_STAGE_2_ALL_TRANSFER_BIT,

    AllCommands                 = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT,
};

enum class MemoryAccess: VkAccessFlags2 {
    IndirectCommandRead         = VK_ACCESS_2_INDIRECT_COMMAND_READ_BIT,

    IndexRead                   = VK_ACCESS_2_INDEX_READ_BIT,
    VertexAttributeRead         = VK_ACCESS_2_VERTEX_ATTRIBUTE_READ_BIT,
    VertexRead                  = IndexRead | VertexAttributeRead,

    ShaderUniformRead           = VK_ACCESS_2_UNIFORM_READ_BIT,
    ShaderSampledRead           = VK_ACCESS_2_SHADER_SAMPLED_READ_BIT,
    ShaderStorageRead           = VK_ACCESS_2_SHADER_STORAGE_READ_BIT,
    ShaderRead                  = VK_ACCESS_2_SHADER_READ_BIT,

    ShaderStorageWrite          = VK_ACCESS_2_SHADER_STORAGE_WRITE_BIT,
    ShaderWrite                 = VK_ACCESS_2_SHADER_WRITE_BIT,

    ColorAttachmentRead         = VK_ACCESS_2_COLOR_ATTACHMENT_READ_BIT,
    DepthAttachmentRead         = VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_READ_BIT,
    StencilAttachmentRead       = VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_READ_BIT,
    DepthStencilAttachmentRead  = DepthAttachmentRead | StencilAttachmentRead,
    AttachmentRead              = ColorAttachmentRead | DepthStencilAttachmentRead,

    ColorAttachmentWrite        = VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT,
    DepthAttachmentWrite        = VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
    StencilAttachmentWrite      = VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
    DepthStencilAttachmentWrite = DepthAttachmentWrite | StencilAttachmentWrite,
    AttachmentWrite             = ColorAttachmentWrite | DepthStencilAttachmentWrite,

    TransferRead                = VK_ACCESS_2_TRANSFER_READ_BIT,
    TransferWrite               = VK_ACCESS_2_TRANSFER_WRITE_BIT,

    MemoryRead                  = VK_ACCESS_2_MEMORY_READ_BIT,
    MemoryWrite                 = VK_ACCESS_2_MEMORY_WRITE_BIT,
};
}
