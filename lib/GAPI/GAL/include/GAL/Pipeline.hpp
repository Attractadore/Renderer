#pragma once
#if GAL_USE_VULKAN
#include "VulkanPipeline.hpp"
#endif

#include "Common/Flags.hpp"
#include "Context.hpp"

namespace R1::GAL {
using ColorComponentFlags = Flags<ColorComponent>;

struct PipelineLayoutConfig {};

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

PipelineLayout CreatePipelineLayout(Context ctx, const PipelineLayoutConfig& config);
void DestroyPipelineLayout(Context ctx, PipelineLayout layout);

void CreateGraphicsPipelines(
    Context ctx, PipelineCache pipeline_cache,
    const GraphicsPipelineConfigs& configs,
    Pipeline* out
);
void DestroyPipeline(Context ctx, Pipeline pipeline);
}
