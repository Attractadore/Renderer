#pragma once
#include "Common/Ref.hpp"
#include "VKRAII.hpp"
#include "VKShaderModule.hpp"
#include "VKUtil.hpp"

#include <array>
#include <functional>
#include <optional>
#include <string>
#include <variant>
#include <vector>

// Thanks XOrg
#undef None
#undef Always

namespace R1::VK {
struct PipelineLayout {
    VkPipelineLayout layout;
};

struct Pipeline: RefedBase<Pipeline> {
    Vk::Pipeline m_pipeline;

protected:
    Pipeline(Vk::Pipeline pipeline):
        m_pipeline{std::move(pipeline)} {}
};
using PipelineRef = Ref<Pipeline>;

enum class VertexInputRate {
    Vertex      = VK_VERTEX_INPUT_RATE_VERTEX,
    Instance    = VK_VERTEX_INPUT_RATE_INSTANCE,
};

enum class VertexFormat {
    Float   = VK_FORMAT_R32_SFLOAT,
    Float2  = VK_FORMAT_R32G32_SFLOAT,
    Float3  = VK_FORMAT_R32G32B32_SFLOAT,
    Float4  = VK_FORMAT_R32G32B32A32_SFLOAT,
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
};

enum class PrimitiveTopologyClass {
    Line        = VK_PRIMITIVE_TOPOLOGY_LINE_LIST,
    Triangle    = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
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

enum class DepthFormat {};

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

enum class StencilFormat {};

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

enum class ColorFormat {
    RGB8_UNORM      = VK_FORMAT_R8G8B8_UNORM,
    RGBA8_UNORM     = VK_FORMAT_R8G8B8A8_UNORM,
    BGR8_UNORM      = VK_FORMAT_B8G8R8_UNORM,
    BGRA8_UNORM     = VK_FORMAT_B8G8R8A8_UNORM,
};

namespace Detail {
enum class PipelineStateDynamicT { Enable };

template<typename T>
class PipelineState: public std::variant<T, PipelineStateDynamicT> {
private:
    using D = PipelineStateDynamicT;
    using Base = std::variant<T, D>;

public:
    using Base::Base;

    bool isDynamic() const {
        return std::holds_alternative<D>(*this);
    }

    template<typename StaticVisitor, typename DynamicOp>
    void visit_or(StaticVisitor&& sv, DynamicOp&& op) const {
        if (not isDynamic()) {
            std::invoke(
                std::forward<StaticVisitor>(sv),
                std::get<T>(*this)
            );
        } else {
            std::invoke(std::forward<DynamicOp>(op));
        }
    }

    template<typename StaticVisitor>
    void visit_if(StaticVisitor&& sv) const {
        if (not isDynamic()) {
            std::invoke(
                std::forward<StaticVisitor>(sv),
                std::get<T>(*this)
            );
        }
    }
};
}
inline constexpr auto PipelineStateDynamic =
    Detail::PipelineStateDynamicT::Enable;

struct ShaderStageInfo {
    ShaderModule        module;
    std::string_view    entry_point = "main";
};

struct VertexInputBindingDescription {
    unsigned        binding;
    unsigned        stride;
    VertexInputRate input_rate = VertexInputRate::Vertex;
};

struct VertexInputAttributeDescription {
    unsigned        location;
    unsigned        binding;
    VertexFormat    format;
    unsigned        offset;
};

struct VertexInputInfo {
    bool dynamic_stride = false;
};

struct InputAssemblyInfo {
    std::variant<PrimitiveTopology,
        PrimitiveTopologyClass>     primitive_topology =
                                        PrimitiveTopology::TriangleList;
    Detail::PipelineState<bool>     primitive_restart_enabled = false;
};

struct TesselationInfo {
    unsigned patch_control_point_count;
};

struct RasterizationInfo {
    struct DepthBiasInfo {
        bool                            dynamic = false;
        struct Params {
            float                       constant_factor;
            float                       clamp = 0.0f;
            float                       slope_factor;
        };
        Detail::PipelineState<Params>   params;
    };

    bool                                dynamic_discard = false;
    PolygonMode                         polygon_mode    = PolygonMode::Fill;
    Detail::PipelineState<CullMode>     cull_mode       = CullMode::None;
    Detail::PipelineState<FrontFace>    front_face      = FrontFace::CounterClockwise;
    std::optional<DepthBiasInfo>        depth_bias;
    Detail::PipelineState<float>        line_width      = 1.0f;
};

struct MultisampleInfo {
    struct SampleShadingInfo {
        float min;
    };

    unsigned                            sample_count        = 1;
    std::optional<SampleShadingInfo>    sample_shading;
    std::optional<uint64_t>             sample_mask;
    bool                                alpha_to_coverage   = false;
    bool                                alpha_to_one        = false;
};

struct DepthTestInfo {
    struct BoundsTestInfo {
        bool                            dynamic = false;
        struct Params {
            float                       min = 0.0f;
            float                       max = 1.0f;
        };
        Detail::PipelineState<Params>   params;
    };

    bool                                dynamic     = false;
    Detail::PipelineState<bool>         write       = true;
    Detail::PipelineState<CompareOp>    compare_op  = CompareOp::Less;
    bool                                clamp       = false;
    std::optional<BoundsTestInfo>       bounds_test;
};

struct DepthAttachmentInfo {
    DepthFormat format;
};

struct StencilTestInfo {
    struct OpInfo {
        struct {
            StencilOp   fail;
            StencilOp   pass;
            StencilOp   depth_fail;
            CompareOp   compare;
        } front, back;
    };

    struct CompareMaskInfo {
        uint32_t front, back;
    };

    struct WriteMaskInfo {
        uint32_t front, back;
    };

    struct ReferenceInfo {
        unsigned front, back;
    };

    bool                                    dynamic = false;
    Detail::PipelineState<OpInfo>           ops;
    Detail::PipelineState<CompareMaskInfo>  compare_mask;
    Detail::PipelineState<WriteMaskInfo>    write_mask;
    Detail::PipelineState<ReferenceInfo>    reference;
};

struct StencilAttachmentInfo {
    StencilFormat format;
};

struct ColorBlendInfo {
    std::optional<LogicOp>                              logic_op;
    std::optional<
        Detail::PipelineState<std::array<float, 4>>>    constants;
};

struct ColorAttachmentInfo {
    struct ComponentFlags {
        bool r: 1,
             g: 1,
             b: 1,
             a: 1;
    };

    struct BlendInfo {
        BlendFactor src_color_factor;
        BlendFactor dst_color_factor;
        BlendOp     color_op;
        BlendFactor src_alpha_factor;
        BlendFactor dst_alpha_factor;
        BlendOp     alpha_op;
    };

    ColorFormat                 format;
    ComponentFlags              color_mask = {1, 1, 1, 1};
    std::optional<BlendInfo>    blend;
};

struct GraphicsPipelineConfig {
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

    std::array<
        std::string, stage_count
    > shader_entry_points;
    std::array<
        VkPipelineShaderStageCreateInfo,
        stage_count
    > shader_stages;

    VkPipelineRenderingCreateInfo           rendering {
        .sType = sType(rendering),
    };
    VkPipelineVertexInputStateCreateInfo    vertex_input_state;
    VkPipelineInputAssemblyStateCreateInfo  input_assembly_state;
    VkPipelineTessellationStateCreateInfo   tesselation_state;
    VkPipelineViewportStateCreateInfo       viewport_state {
        .sType = sType(viewport_state),
    };
    VkPipelineRasterizationStateCreateInfo  rasterization_state {
        .sType = sType(rasterization_state),
        .rasterizerDiscardEnable = true,
    };
    VkPipelineMultisampleStateCreateInfo    multisample_state;
    uint64_t                                sample_mask;
    VkPipelineDepthStencilStateCreateInfo   depth_stencil_state {
        .sType = sType(depth_stencil_state),
    };
    VkPipelineColorBlendStateCreateInfo     color_blend_state;
    VkPipelineDynamicStateCreateInfo        dynamic_state;

    bool dynamic_vertex_input_binding_stride: 1;
    bool dynamic_primitive_topology: 1;
    bool dynamic_primitive_restart: 1;
    bool dynamic_rasterizer_discard: 1;
    bool dynamic_cull_mode: 1;
    bool dynamic_front_face: 1;
    bool dynamic_line_width: 1;
    bool dynamic_depth_bias: 1;
    bool dynamic_depth_bias_params: 1;
    bool dynamic_depth_test: 1;
    bool dynamic_depth_write: 1;
    bool dynamic_depth_compare_op: 1;
    bool dynamic_depth_bounds_test: 1;
    bool dynamic_depth_bounds_test_params: 1;
    bool dynamic_stencil_test: 1;
    bool dynamic_stencil_test_ops: 1;
    bool dynamic_stencil_test_compare_mask: 1;
    bool dynamic_stencil_test_write_mask: 1;
    bool dynamic_stencil_test_reference: 1;
    bool dynamic_blend_constants: 1;
};

struct GraphicsPipelineConfigs {
    std::vector<GraphicsPipelineConfig>
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

class GraphicsPipelineConfigurator {
    GraphicsPipelineConfigs         m_configs;            
    GraphicsPipelineConfig          m_current_config = {};
    VkGraphicsPipelineCreateInfo    m_current_create_info = {};

public:
    GraphicsPipelineConfigurator& SetLayout(PipelineLayout layout);
    GraphicsPipelineConfigurator& SetVertexShaderState(
        const ShaderStageInfo& vertex_shader_info,
        const VertexInputInfo& vertex_input_info,
        std::initializer_list<VertexInputBindingDescription>
            vertex_input_binding_descriptions,
        std::initializer_list<VertexInputAttributeDescription>
            vertex_input_attribute_descriptions,
        const InputAssemblyInfo& input_assembly_info
    );
    GraphicsPipelineConfigurator& SetTessellationShaderState(
        const ShaderStageInfo& tesselation_control_shader_info,
        const ShaderStageInfo& tesselation_evaluation_shader_info,
        const TesselationInfo& tesselation_info
    );
    GraphicsPipelineConfigurator& SetGeometryShaderState(
        const ShaderStageInfo& geometry_shader_info
    );
    GraphicsPipelineConfigurator& SetRasterizationState(
        const RasterizationInfo& rasterization_info,
        const MultisampleInfo& multisample_info
    );
    GraphicsPipelineConfigurator& SetDepthTestState(
        const DepthTestInfo& depth_test_info,
        const DepthAttachmentInfo& depth_attachment_info
    );
    GraphicsPipelineConfigurator& SetStencilTestState(
        const StencilTestInfo& stencil_test_info,
        const StencilAttachmentInfo& stencil_attachment_info
    );
    GraphicsPipelineConfigurator& SetFragmentShaderState(
        const ShaderStageInfo& fragment_shader_info,
        const ColorBlendInfo& color_blend_info,
        std::initializer_list<ColorAttachmentInfo>
            color_attachment_infos
    );

    GraphicsPipelineConfigurator& FinishCurrent();

    GraphicsPipelineConfigs FinishAll();
};
}
