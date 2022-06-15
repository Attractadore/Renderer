#pragma once
#include "Types.hpp"

namespace R1::GAL {
using ShaderStageConfig             = GraphicsPipelineConfigurator::ShaderStageConfig;
using VertexInputBindingConfig      = GraphicsPipelineConfigurator::VertexInputBindingConfig;
using VertexInputAttributeConfig    = GraphicsPipelineConfigurator::VertexInputAttributeConfig;
using InputAssemblyConfig           = GraphicsPipelineConfigurator::InputAssemblyConfig;
using RasterizationConfig           = GraphicsPipelineConfigurator::RasterizationConfig;
using DepthTestConfig               = GraphicsPipelineConfigurator::DepthTestConfig;
using DepthAttachmentConfig         = GraphicsPipelineConfigurator::DepthAttachmentConfig;
using StencilTestConfig             = GraphicsPipelineConfigurator::StencilTestConfig;
using StencilAttachmentConfig       = GraphicsPipelineConfigurator::StencilAttachmentConfig;
using ColorBlendConfig              = GraphicsPipelineConfigurator::ColorBlendConfig;
using ColorAttachmentConfig         = GraphicsPipelineConfigurator::ColorAttachmentConfig;

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
