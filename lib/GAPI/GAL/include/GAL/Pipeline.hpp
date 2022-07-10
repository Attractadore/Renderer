#pragma once
#if GAL_USE_VULKAN
#include "VulkanPipeline.hpp"
#endif

#include "Common/Flags.hpp"
#include "Context.hpp"
#include "Descriptors.hpp"

namespace R1::GAL {
namespace Detail {
template<typename E>
concept IsVertexInputRate = requires(E e) {
    E::Vertex;
    E::Instance;
};

template<typename E>
concept IsPrimitiveTopology = requires(E e) {
    E::PointList;
    E::LineList;
    E::LineListWithAdjacency;
    E::LineStrip;
    E::LineStripWithAdjacency;
    E::TriangleList;
    E::TriangleListWithAdjacency;
    E::TriangleStrip;
    E::TriangleStripWithAdjacency;
    E::TriangleFan;
    E::PatchList;

    E::Points;
    E::Lines;
    E::Triangles;
};

template<typename E>
concept IsPolygonMode = requires(E e) {
    E::Fill;
    E::Line;
    E::Point;
};

template<typename E>
concept IsCullMode = requires(E e) {
    E::None;
    E::Front;
    E::Back;
    E::FrontAndBack;
};

template<typename E>
concept IsFrontFace = requires(E e) {
    E::CounterClockwise;
    E::Clockwise; 
};

template<typename E>
concept IsCompareOp = requires(E e) {
    E::Always;    
    E::Never;
    E::Equal;
    E::NotEqual;
    E::Less;
    E::LessOrEqual;
    E::Greater;
    E::GreaterOrEqual;
};

template<typename E>
concept IsStencilOp = requires(E e) {
    E::Keep;
    E::Replace;
    E::Zero;
    E::Invert;
    E::IncrementAndClamp;
    E::IncrementAndWrap;
    E::DecrementAndClamp;
    E::DecrementAndWrap;
};

template<typename E>
concept IsLogicOp = requires(E e) {
    E::NoOp;
    E::Clear;
    E::Set;    
    E::Invert;
    E::Equivalent;
    E::And;
    E::AndReverse;
    E::AndInverted;
    E::Nand;
    E::Or;
    E::OrInverted;
    E::OrReverse;
    E::Nor;
    E::Xor;
    E::Copy;
    E::CopyInverted;
};

template<typename E>
concept IsColorComponent = requires(E e) {
    E::R;
    E::G;
    E::B;
    E::A;
};

template<typename E>
concept IsBlendFactor = requires(E e) {
    E::Zero;
    E::One;
    E::SrcColor;
    E::OneMinusSrcColor;
    E::DstColor;
    E::OneMinusDstColor;
    E::SrcAlpha;
    E::OneMinusSrcAlpha;
    E::DstAlpha;
    E::OneMinusDstAlpha;
    E::ConstColor;
    E::OneMinusConstColor;
    E::ConstAlpha;
    E::OneMinusConstAlpha;
    E::SrcAlphaSaturate;
    E::Src1Color;
    E::OneMinusSrc1Color;
    E::Src1Alpha;
    E::OneSrc1Alpha;
};

template<typename E>
concept IsBlendOp = requires(E e) {
    E::Add;
    E::Subtract;
    E::ReverseSubtract;
    E::Min;
    E::Max;
};

static_assert(IsVertexInputRate<VertexInputRate>);
static_assert(IsPrimitiveTopology<PrimitiveTopology>);
static_assert(IsPolygonMode<PolygonMode>);
static_assert(IsCullMode<CullMode>);
static_assert(IsFrontFace<FrontFace>);
static_assert(IsCompareOp<CompareOp>);
static_assert(IsStencilOp<StencilOp>);
static_assert(IsLogicOp<LogicOp>);
static_assert(IsColorComponent<ColorComponent>);
static_assert(IsBlendFactor<BlendFactor>);
static_assert(IsBlendOp<BlendOp>);
}

using ColorComponentFlags = Flags<ColorComponent>;

struct ShaderModuleConfig {
    std::span<const std::byte> code;
};

struct ShaderStageConfig {
    ShaderModule        module;
    std::string_view    entry_point;
};

struct VertexInputBindingConfig {
    unsigned        binding;
    unsigned        stride;
    VertexInputRate input_rate;
};

struct VertexInputAttributeConfig {
    unsigned        location;
    unsigned        binding;
    Format  format;
    unsigned        offset;
};

struct VertexInputConfig {};

struct InputAssemblyConfig {
    PrimitiveTopology   primitive_topology;
    bool                primitive_restart_enabled: 1;
};

struct TesselationConfig {
    unsigned patch_control_point_count;
};

struct RasterizationConfig {
    PolygonMode polygon_mode;
    CullMode    cull_mode;
    FrontFace   front_face;
    struct {
        float   constant_factor;
        float   clamp;
        float   slope_factor;
    }           depth_bias;
    float       line_width;
    bool        discard: 1;
    bool        depth_bias_enabled: 1;
};

struct MultisampleConfig {
    unsigned        sample_count;
    struct {
        float       min;
    }               sample_shading;
    const uint64_t* sample_mask;
    bool            alpha_to_coverage: 1;
    bool            alpha_to_one: 1;
    bool            sample_shading_enabled: 1;
};

struct DepthTestConfig {
    CompareOp   compare_op;
    struct {
        float   min;
        float   max;
    }           bounds_test;
    bool        enabled: 1;
    bool        write_enabled: 1;
    bool        clamp_enabled: 1;
    bool        bounds_test_enabled: 1;
};

struct DepthAttachmentConfig {
    Format format;
};

struct StencilTestConfig {
    struct {
        StencilOp   fail_op;
        StencilOp   pass_op;
        StencilOp   depth_fail_op;
        CompareOp   compare_op;
        uint32_t    compare_mask;
        uint32_t    write_mask;
        uint32_t    reference;
    }               front, back;
    bool            enabled: 1;
};

struct StencilAttachmentConfig {
    Format format;
};

struct ColorBlendConfig {
    LogicOp                 logic_op;
    std::array<float, 4>    constants;
    bool                    logic_op_enabled: 1;
};

struct ColorAttachmentConfig {
    Format              format;
    ColorComponentFlags color_mask;
    struct {
        BlendFactor     src_color_factor;
        BlendFactor     dst_color_factor;
        BlendOp         color_op;
        BlendFactor     src_alpha_factor;
        BlendFactor     dst_alpha_factor;
        BlendOp         alpha_op;
        bool            enabled: 1;
    }                   blend;
};

enum class Dynamic {
    VertexInputBindingStride    = 1 << 0,
    PrimitiveTopology           = 1 << 1,
    PrimitiveRestart            = 1 << 2,
    RasterizerDiscard           = 1 << 3,
    CullMode                    = 1 << 4,
    FrontFace                   = 1 << 5,
    LineWidth                   = 1 << 6,
    DepthBias                   = 1 << 7,
    DepthBiasParams             = 1 << 8,
    DepthTest                   = 1 << 9,
    DepthWrite                  = 1 << 10,
    DepthCompareOp              = 1 << 11,
    DepthBoundsTest             = 1 << 12,
    DepthBoundsTestParams       = 1 << 13,
    StencilTest                 = 1 << 14,
    StencilTestOps              = 1 << 15,
    StencilTestCompareMask      = 1 << 16,
    StencilTestWriteMask        = 1 << 17,
    StencilTestReference        = 1 << 18,
    BlendConstants              = 1 << 19,
};
using DynamicStateFlags = Flags<Dynamic>;

struct GraphicsPipelineConfigurator: private Detail::GraphicsPipelineConfiguratorData {
    GraphicsPipelineConfigurator(): Detail::GraphicsPipelineConfiguratorData{} {}
    GraphicsPipelineConfigurator& SetLayout(PipelineLayout layout);
    GraphicsPipelineConfigurator& SetVertexShaderState(
        const ShaderStageConfig& vertex_shader_config,
        const VertexInputConfig& vertex_input_config,
        std::span<const VertexInputBindingConfig> vertex_binding_configs,
        std::span<const VertexInputAttributeConfig> vertex_attribute_configs,
        const InputAssemblyConfig& input_assembly_config
    );
    GraphicsPipelineConfigurator& SetTessellationShaderState(
        const ShaderStageConfig& tesselation_control_shader_config,
        const ShaderStageConfig& tesselation_evaluation_shader_config,
        const TesselationConfig& tesselation_config
    );
    GraphicsPipelineConfigurator& SetGeometryShaderState(
        const ShaderStageConfig& geometry_shader_config
    );
    GraphicsPipelineConfigurator& SetRasterizationState(
        const RasterizationConfig& rasterization_config,
        const MultisampleConfig& multisample_config
    );
    GraphicsPipelineConfigurator& SetDepthTestState(
        const DepthTestConfig& depth_test_config,
        const DepthAttachmentConfig& depth_attachment_config
    );
    GraphicsPipelineConfigurator& SetStencilTestState(
        const StencilTestConfig& stencil_test_config,
        const StencilAttachmentConfig& stencil_attachment_config
    );
    GraphicsPipelineConfigurator& SetFragmentShaderState(
        const ShaderStageConfig& fragment_shader_config,
        const ColorBlendConfig& color_blend_config,
        std::span<const ColorAttachmentConfig> color_attachment_configs
    );
    GraphicsPipelineConfigurator& SetDynamicState(
        DynamicStateFlags flags 
    );

    GraphicsPipelineConfigurator& FinishCurrent();
    GraphicsPipelineConfigs       FinishAll();
};

ShaderModule CreateShaderModule(Context ctx, const ShaderModuleConfig& config);
void DestroyShaderModule(Context ctx, ShaderModule module);

struct PipelineLayoutConfig {
    std::span<const DescriptorSetLayout> descriptor_set_layouts;
};

PipelineLayout CreatePipelineLayout(Context ctx, const PipelineLayoutConfig& config);
void DestroyPipelineLayout(Context ctx, PipelineLayout layout);

void CreateGraphicsPipelines(
    Context ctx, PipelineCache pipeline_cache,
    const GraphicsPipelineConfigs& configs,
    Pipeline* out
);
void DestroyPipeline(Context ctx, Pipeline pipeline);
}
