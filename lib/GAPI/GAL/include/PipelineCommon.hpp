#pragma once
#include <array>
#include <cstdint>
#include <span>
#include <string_view>

namespace R1::GAL {
struct ShaderModuleConfig {
    std::span<const std::byte> code;
};

struct PipelineLayoutConfig {};

namespace Detail {
template<typename Traits>
struct ShaderStageConfigBase {
    Traits::ShaderModule    module;
    std::string_view        entry_point;
};

template<typename Traits>
struct VertexInputBindingConfigBase {
    unsigned                binding;
    unsigned                stride;
    Traits::VertexInputRate input_rate;
};

template<typename Traits>
struct VertexInputAttributeConfigBase {
    unsigned        location;
    unsigned        binding;
    Traits::Format  format;
    unsigned        offset;
};
}

struct VertexInputConfig {
    bool dynamic_stride: 1;
};

namespace Detail {
template<typename Traits>
struct InputAssemblyConfigBase {
    Traits::PrimitiveTopology   primitive_topology;
    bool                        primitive_restart_enabled: 1;
    bool                        dynamic_primitive_topology: 1;
    bool                        dynamic_primitive_restart: 1;
};
}

struct TesselationConfig {
    unsigned patch_control_point_count;
};

namespace Detail {
template<typename Traits>
struct RasterizationConfigBase {
    Traits::PolygonMode polygon_mode;
    Traits::CullMode    cull_mode;
    Traits::FrontFace   front_face;
    struct {
        float           constant_factor;
        float           clamp;
        float           slope_factor;
        bool            enabled: 1;
        bool            dynamic: 1;
        bool            dynamic_values: 1;
    }                   depth_bias;
    float               line_width;
    bool                discard: 1;
    bool                dynamic_discard: 1;
    bool                dynamic_cull_mode: 1;
    bool                dynamic_front_face: 1;
    bool                dynamic_line_width: 1;
};
}

struct MultisampleConfig {
    unsigned        sample_count;
    struct {
        float       min;
        bool        enabled: 1;
    }               sample_shading;
    const uint64_t* sample_mask;
    bool            alpha_to_coverage: 1;
    bool            alpha_to_one: 1;
};

namespace Detail {
template<typename Traits>
struct DepthTestConfigBase {
    Traits::CompareOp   compare_op;
    struct {
        float           min;
        float           max;
        bool            enabled: 1;
        bool            dynamic: 1;
        bool            dynamic_values: 1;
    }                   bounds_test;
    bool                enabled: 1;
    bool                write_enabled: 1;
    bool                clamp_enabled: 1;
    bool                dynamic: 1;
    bool                dynamic_write: 1;
    bool                dynamic_compare_op: 1;
};

template<typename Traits>
struct DepthAttachmentConfigBase {
    Traits::Format format;
};

template<typename Traits>
struct StencilTestConfigBase {
    struct {
        Traits::StencilOp   fail_op;
        Traits::StencilOp   pass_op;
        Traits::StencilOp   depth_fail_op;
        Traits::CompareOp   compare_op;
        uint32_t            compare_mask;
        uint32_t            write_mask;
        uint32_t            reference;
    }                       front, back;
    bool                    enabled: 1;
    bool                    dynamic: 1;
    bool                    dynamic_op: 1;
    bool                    dynamic_compare_mask: 1;
    bool                    dynamic_write_mask: 1;
    bool                    dynamic_reference: 1;
};

template<typename Traits>
struct StencilAttachmentConfigBase {
    Traits::Format format;
};

template<typename Traits>
struct ColorBlendConfigBase {
    Traits::LogicOp         logic_op;
    std::array<float, 4>    constants;
    bool                    logic_op_enabled: 1;
    bool                    dynamic_constants: 1;
};

template<typename Traits>
struct ColorAttachmentConfigBase {
    Traits::Format              format;
    Traits::ColorComponentFlags color_mask;
    struct {
        Traits::BlendFactor     src_color_factor;
        Traits::BlendFactor     dst_color_factor;
        Traits::BlendOp         color_op;
        Traits::BlendFactor     src_alpha_factor;
        Traits::BlendFactor     dst_alpha_factor;
        Traits::BlendOp         alpha_op;
        bool                    enabled: 1;
    }                           blend;
};

struct GraphicsPipelineConfigBase {
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

template<class Data, class Traits>
struct GraphicsPipelineConfiguratorBase: private Data {
    using ShaderStageConfig = Detail::ShaderStageConfigBase<Traits>;
    using VertexInputBindingConfig = Detail::VertexInputBindingConfigBase<Traits>;
    using VertexInputAttributeConfig = Detail::VertexInputAttributeConfigBase<Traits>;
    using InputAssemblyConfig = Detail::InputAssemblyConfigBase<Traits>;
    using RasterizationConfig = Detail::RasterizationConfigBase<Traits>;
    using DepthTestConfig = Detail::DepthTestConfigBase<Traits>;
    using DepthAttachmentConfig = Detail::DepthAttachmentConfigBase<Traits>;
    using StencilTestConfig = Detail::StencilTestConfigBase<Traits>;
    using StencilAttachmentConfig = Detail::StencilAttachmentConfigBase<Traits>;
    using ColorBlendConfig = Detail::ColorBlendConfigBase<Traits>;
    using ColorAttachmentConfig = Detail::ColorAttachmentConfigBase<Traits>;

    GraphicsPipelineConfiguratorBase(): Data{} {}
    GraphicsPipelineConfiguratorBase& SetLayout(Traits::PipelineLayout layout);
    GraphicsPipelineConfiguratorBase& SetVertexShaderState(
        const ShaderStageConfig& vertex_shader_config,
        const VertexInputConfig& vertex_input_config,
        std::span<const VertexInputBindingConfig> vertex_binding_configs,
        std::span<const VertexInputAttributeConfig> vertex_attribute_configs,
        const InputAssemblyConfig& input_assembly_config
    );
    GraphicsPipelineConfiguratorBase& SetTessellationShaderState(
        const ShaderStageConfig& tesselation_control_shader_config,
        const ShaderStageConfig& tesselation_evaluation_shader_config,
        const TesselationConfig& tesselation_config
    );
    GraphicsPipelineConfiguratorBase& SetGeometryShaderState(
        const ShaderStageConfig& geometry_shader_config
    );
    GraphicsPipelineConfiguratorBase& SetRasterizationState(
        const RasterizationConfig& rasterization_config,
        const MultisampleConfig& multisample_config
    );
    GraphicsPipelineConfiguratorBase& SetDepthTestState(
        const DepthTestConfig& depth_test_config,
        const DepthAttachmentConfig& depth_attachment_config
    );
    GraphicsPipelineConfiguratorBase& SetStencilTestState(
        const StencilTestConfig& stencil_test_config,
        const StencilAttachmentConfig& stencil_attachment_config
    );
    GraphicsPipelineConfiguratorBase& SetFragmentShaderState(
        const ShaderStageConfig& fragment_shader_config,
        const ColorBlendConfig& color_blend_config,
        std::span<const ColorAttachmentConfig> color_attachment_configs
    );

    GraphicsPipelineConfiguratorBase& FinishCurrent();
    Traits::GraphicsPipelineConfigs   FinishAll();
};
}
}
