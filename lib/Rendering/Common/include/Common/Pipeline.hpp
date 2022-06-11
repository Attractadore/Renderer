#pragma once
#include "Format.hpp"

#include <span>
#include <string_view>

namespace R1 {
struct ShaderModuleConfig {
    std::span<const std::byte> code;
};

struct PipelineLayoutConfig {};

enum class VertexInputRate;
enum class PrimitiveTopology;
enum class PolygonMode;
enum class CullMode;
enum class FrontFace;
enum class CompareOp;
enum class StencilOp;
enum class LogicOp;
enum class BlendFactor;
enum class BlendOp;

template<class ShaderModuleT>
struct ShaderStageInfoBase {
    ShaderModuleT       module;
    std::string_view    entry_point;
};

struct VertexInputBindingDescription {
    unsigned        binding;
    unsigned        stride;
    VertexInputRate input_rate;
};

struct VertexInputAttributeDescription {
    unsigned        location;
    unsigned        binding;
    Format          format;
    unsigned        offset;
};

struct VertexInputInfo {
    bool dynamic_stride: 1;
};

struct InputAssemblyInfo {
    PrimitiveTopology   primitive_topology;
    bool                primitive_restart_enabled: 1;
    bool                dynamic_primitive_topology: 1;
    bool                dynamic_primitive_restart: 1;
};

struct TesselationInfo {
    unsigned patch_control_point_count;
};

struct RasterizationInfo {
    PolygonMode polygon_mode;
    CullMode    cull_mode;
    FrontFace   front_face;
    struct {
        float   constant_factor;
        float   clamp;
        float   slope_factor;
        bool    enabled: 1;
        bool    dynamic: 1;
        bool    dynamic_values: 1;
    }           depth_bias;
    float       line_width;
    bool        discard: 1;
    bool        dynamic_discard: 1;
    bool        dynamic_cull_mode: 1;
    bool        dynamic_front_face: 1;
    bool        dynamic_line_width: 1;
};

struct MultisampleInfo {
    unsigned        sample_count;
    struct {
        float       min;
        bool        enabled: 1;
    }               sample_shading;
    const uint64_t* sample_mask;
    bool            alpha_to_coverage: 1;
    bool            alpha_to_one: 1;
};

struct DepthTestInfo {
    CompareOp   compare_op;
    struct {
        float   min;
        float   max;
        bool    enabled: 1;
        bool    dynamic: 1;
        bool    dynamic_values: 1;
    }           bounds_test;
    bool        enabled: 1;
    bool        write_enabled: 1;
    bool        clamp_enabled: 1;
    bool        dynamic: 1;
    bool        dynamic_write: 1;
    bool        dynamic_compare_op: 1;
};

struct DepthAttachmentInfo {
    Format format;
};

struct StencilTestInfo {
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
    bool            dynamic: 1;
    bool            dynamic_op: 1;
    bool            dynamic_compare_mask: 1;
    bool            dynamic_write_mask: 1;
    bool            dynamic_reference: 1;
};

struct StencilAttachmentInfo {
    Format format;
};

struct ColorBlendInfo {
    LogicOp                 logic_op;
    std::array<float, 4>    constants;
    bool                    logic_op_enabled: 1;
    bool                    dynamic_constants: 1;
};

struct ColorAttachmentInfo {
    Format          format;
    struct ComponentMask {
        bool        r: 1, g: 1, b: 1, a: 1;
    }               color_mask;
    struct {
        BlendFactor src_color_factor;
        BlendFactor dst_color_factor;
        BlendOp     color_op;
        BlendFactor src_alpha_factor;
        BlendFactor dst_alpha_factor;
        BlendOp     alpha_op;
        bool        enabled: 1;
    } blend;
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
}
