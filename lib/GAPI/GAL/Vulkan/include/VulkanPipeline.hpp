#pragma once
#include "PipelineCommon.hpp"
#include "VulkanFormat.hpp"

#include <array>
#include <string>
#include <vector>

namespace R1::GAL {
enum class VertexInputRate {
    Vertex      = VK_VERTEX_INPUT_RATE_VERTEX,
    Instance    = VK_VERTEX_INPUT_RATE_INSTANCE,
};

enum class PrimitiveTopology {
    PointList                   = VK_PRIMITIVE_TOPOLOGY_POINT_LIST,
    LineList                    = VK_PRIMITIVE_TOPOLOGY_LINE_LIST,
    LineListWithAdjacency       = VK_PRIMITIVE_TOPOLOGY_LINE_LIST_WITH_ADJACENCY,
    LineStrip                   = VK_PRIMITIVE_TOPOLOGY_LINE_STRIP,
    LineStripWithAdjacency      = VK_PRIMITIVE_TOPOLOGY_LINE_STRIP_WITH_ADJACENCY,
    TriangleList                = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
    TriangleListWithAdjacency   = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST_WITH_ADJACENCY,
    TriangleStrip               = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP,
    TriangleStripWithAdjacency  = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP_WITH_ADJACENCY,
    TriangleFan                 = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN,
    PatchList                   = VK_PRIMITIVE_TOPOLOGY_PATCH_LIST,

    Points                      = PointList,
    Lines                       = LineList,
    Triangles                   = TriangleList,
};

enum class PolygonMode {
    Fill    = VK_POLYGON_MODE_FILL,
    Line    = VK_POLYGON_MODE_LINE,
    Point   = VK_POLYGON_MODE_POINT
};

enum class CullMode {
    None            = VK_CULL_MODE_NONE,
    Front           = VK_CULL_MODE_FRONT_BIT,
    Back            = VK_CULL_MODE_BACK_BIT,
    FrontAndBack    = VK_CULL_MODE_FRONT_AND_BACK,
};

enum class FrontFace {
    CounterClockwise    = VK_FRONT_FACE_COUNTER_CLOCKWISE,
    Clockwise           = VK_FRONT_FACE_CLOCKWISE, 
};

enum class CompareOp {
    Always          = VK_COMPARE_OP_ALWAYS,    
    Never           = VK_COMPARE_OP_NEVER,
    Equal           = VK_COMPARE_OP_EQUAL,
    NotEqual        = VK_COMPARE_OP_NOT_EQUAL,
    Less            = VK_COMPARE_OP_LESS,
    LessOrEqual     = VK_COMPARE_OP_LESS_OR_EQUAL,
    Greater         = VK_COMPARE_OP_GREATER,
    GreaterOrEqual  = VK_COMPARE_OP_GREATER_OR_EQUAL,
};

enum class StencilOp {
    Keep                = VK_STENCIL_OP_KEEP,
    Replace             = VK_STENCIL_OP_REPLACE,
    Zero                = VK_STENCIL_OP_ZERO,
    Invert              = VK_STENCIL_OP_INVERT,
    IncrementAndClamp   = VK_STENCIL_OP_INCREMENT_AND_CLAMP,
    IncrementAndWrap    = VK_STENCIL_OP_INCREMENT_AND_WRAP,
    DecrementAndClamp   = VK_STENCIL_OP_DECREMENT_AND_CLAMP,
    DecrementAndWrap    = VK_STENCIL_OP_DECREMENT_AND_WRAP,
};

enum class LogicOp {
    NoOp            = VK_LOGIC_OP_NO_OP,
    Clear           = VK_LOGIC_OP_CLEAR,
    Set             = VK_LOGIC_OP_SET,    
    Invert          = VK_LOGIC_OP_INVERT,
    Equivalent      = VK_LOGIC_OP_EQUIVALENT,
    And             = VK_LOGIC_OP_AND,
    AndReverse      = VK_LOGIC_OP_AND_REVERSE,
    AndInverted     = VK_LOGIC_OP_AND_INVERTED,
    Nand            = VK_LOGIC_OP_NAND,
    Or              = VK_LOGIC_OP_OR,
    OrInverted      = VK_LOGIC_OP_OR_INVERTED,
    OrReverse       = VK_LOGIC_OP_OR_REVERSE,
    Nor             = VK_LOGIC_OP_NOR,
    Xor             = VK_LOGIC_OP_XOR,
    Copy            = VK_LOGIC_OP_COPY,
    CopyInverted    = VK_LOGIC_OP_COPY_INVERTED,
};

enum class ColorComponent {
    R = VK_COLOR_COMPONENT_R_BIT,
    G = VK_COLOR_COMPONENT_G_BIT,
    B = VK_COLOR_COMPONENT_B_BIT,
    A = VK_COLOR_COMPONENT_A_BIT,
};

enum class BlendFactor {
    Zero                = VK_BLEND_FACTOR_ZERO,
    One                 = VK_BLEND_FACTOR_ONE,
    SrcColor            = VK_BLEND_FACTOR_SRC_COLOR,
    OneMinusSrcColor    = VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR,
    DstColor            = VK_BLEND_FACTOR_DST_COLOR,
    OneMinusDstColor    = VK_BLEND_FACTOR_ONE_MINUS_DST_COLOR,
    SrcAlpha            = VK_BLEND_FACTOR_SRC_ALPHA,
    OneMinusSrcAlpha    = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
    DstAlpha            = VK_BLEND_FACTOR_DST_ALPHA,
    OneMinusDstAlpha    = VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA,
    ConstColor          = VK_BLEND_FACTOR_CONSTANT_COLOR,
    OneMinusConstColor  = VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_COLOR,
    ConstAlpha          = VK_BLEND_FACTOR_CONSTANT_ALPHA,
    OneMinusConstAlpha  = VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_ALPHA,
    SrcAlphaSaturate    = VK_BLEND_FACTOR_SRC_ALPHA_SATURATE,
    Src1Color           = VK_BLEND_FACTOR_SRC1_COLOR,
    OneMinusSrc1Color   = VK_BLEND_FACTOR_ONE_MINUS_SRC1_COLOR,
    Src1Alpha           = VK_BLEND_FACTOR_SRC1_ALPHA,
    OneSrc1Alpha        = VK_BLEND_FACTOR_ONE_MINUS_SRC1_ALPHA,
};

enum class BlendOp {
    Add             = VK_BLEND_OP_ADD,
    Subtract        = VK_BLEND_OP_SUBTRACT,
    ReverseSubtract = VK_BLEND_OP_REVERSE_SUBTRACT,
    Min             = VK_BLEND_OP_MIN,
    Max             = VK_BLEND_OP_MAX,
};

using ShaderModule = VkShaderModule;
using PipelineLayout = VkPipelineLayout;
using PipelineCache = VkPipelineCache;
using Pipeline = VkPipeline;

namespace Detail {
struct GraphicsPipelineConfig: Detail::GraphicsPipelineConfigBase {
    static constexpr auto stage_count = std::size({
        VK_SHADER_STAGE_VERTEX_BIT,
        VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT,
        VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT,
        VK_SHADER_STAGE_GEOMETRY_BIT,
        VK_SHADER_STAGE_FRAGMENT_BIT,
    });
    static constexpr auto vertex_stage_idx                  = 0;
    static constexpr auto tesselation_control_stage_idx     = 1;
    static constexpr auto tesselation_evaluation_stage_idx  = 2;
    static constexpr auto geometry_stage_idx                = 3;
    static constexpr auto fragment_stage_idx                = 4;

    std::array<std::string, stage_count>    shader_entry_points;
    std::array<
        VkPipelineShaderStageCreateInfo,
        stage_count
    >                                       shader_stages;
    VkPipelineRenderingCreateInfo           rendering;
    VkPipelineVertexInputStateCreateInfo    vertex_input_state;
    VkPipelineInputAssemblyStateCreateInfo  input_assembly_state;
    VkPipelineTessellationStateCreateInfo   tesselation_state;
    VkPipelineViewportStateCreateInfo       viewport_state;
    VkPipelineRasterizationStateCreateInfo  rasterization_state = {
        .rasterizerDiscardEnable = true,
    };
    VkPipelineMultisampleStateCreateInfo    multisample_state;
    uint64_t                                sample_mask;
    VkPipelineDepthStencilStateCreateInfo   depth_stencil_state;
    VkPipelineColorBlendStateCreateInfo     color_blend_state;
    VkPipelineDynamicStateCreateInfo        dynamic_state;
};
}

struct GraphicsPipelineConfigs {
    std::vector<Detail::GraphicsPipelineConfig>
        configs;
    std::vector<VkVertexInputBindingDescription>
        vertex_input_binding_descriptions;
    std::vector<VkVertexInputAttributeDescription>
        vertex_input_attribute_descriptions;
    std::vector<VkPipelineColorBlendAttachmentState>
        color_blend_attachments;
    std::vector<VkFormat>
        color_attachment_formats;
    std::vector<VkDynamicState>
        dynamic_states;
    std::vector<VkGraphicsPipelineCreateInfo>
        create_infos;
};

namespace Detail {
struct GraphicsPipelineConfiguratorData {
    GraphicsPipelineConfigs         m_configs;            
    GraphicsPipelineConfig          m_current_config = {};
    VkGraphicsPipelineCreateInfo    m_current_create_info = {};
};
}
}
