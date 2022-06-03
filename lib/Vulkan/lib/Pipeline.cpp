#include "ContextImpl.hpp"
#include "Pipeline.hpp"
#include "PipelineImpl.hpp"
#include "VKUtil.hpp"

#include <algorithm>
#include <ranges>
#include <utility>

namespace R1::VK {
ShaderModule CreateShaderModule(Context ctx, const ShaderModuleConfig& config) {
    const auto& code = config.code;
    VkShaderModule module = VK_NULL_HANDLE;
    VkShaderModuleCreateInfo create_info = {
        .sType = sType(create_info),
        .codeSize = code.size(),
        .pCode = reinterpret_cast<const uint32_t*>(code.data()),
    };
    vkCreateShaderModule(ctx->device.get(), &create_info, nullptr, &module);
    if (!module) {
        throw std::runtime_error{"Vulkan: Failed to create shader module"};
    }
    return module;
}

PipelineLayout CreatePipelineLayout(Context ctx, const PipelineLayoutConfig& config) {
    VkPipelineLayoutCreateInfo create_info = {
        .sType = sType(create_info),
    };
    VkPipelineLayout layout = VK_NULL_HANDLE;
    vkCreatePipelineLayout(ctx->device.get(), &create_info, nullptr, &layout);
    if (!layout) {
        throw std::runtime_error{"Vulkan: Failed to create pipeline layout\n"};
    }
    return layout;
}

namespace {
void fillShaderStage(
    VkPipelineShaderStageCreateInfo& stage,
    std::string& entry_point,
    const ShaderStageInfo& stage_info, VkShaderStageFlagBits type
) {
    entry_point = stage_info.entry_point;
    stage = {
        .sType = sType(stage),
        .stage = type,
        .module = stage_info.module,
    };
}
}

using GPC = GraphicsPipelineConfigurator;

GPC& GPC::SetLayout(PipelineLayout layout) {
    m_current_create_info.layout = layout;
    return *this;
}

GPC& GPC::SetVertexShaderState(
    const ShaderStageInfo& vertex_shader_info,
    const VertexInputInfo& vertex_input_info,
    std::span<const VertexInputBindingDescription> vertex_binding_descriptions,
    std::span<const VertexInputAttributeDescription> vertex_attribute_descriptions,
    const InputAssemblyInfo& input_assembly_info
) {
    auto& cfg = m_current_config;

    auto idx = cfg.vertex_stage_idx;
    fillShaderStage(
        cfg.shader_stages[idx],
        cfg.shader_entry_points[idx],
        std::move(vertex_shader_info),
        VK_SHADER_STAGE_VERTEX_BIT
    );

    auto& vis = cfg.vertex_input_state;
    const auto& vii = vertex_input_info;
    vis = {
        .sType = sType(vis),
        .vertexBindingDescriptionCount =
            static_cast<uint32_t>(vertex_binding_descriptions.size()),
        .vertexAttributeDescriptionCount = 
            static_cast<uint32_t>(vertex_attribute_descriptions.size()),
    };
    cfg.dynamic_vertex_input_binding_stride = vii.dynamic_stride;

    auto& bindings = m_configs.vertex_input_binding_descriptions;
    auto vb = std::views::transform(vertex_binding_descriptions,
        [] (const VertexInputBindingDescription& desc) {
            return VkVertexInputBindingDescription {
                .binding = desc.binding,
                .stride = desc.stride,
                .inputRate = static_cast<VkVertexInputRate>(desc.input_rate),
            };
        }
    );
    bindings.insert(bindings.end(), vb.begin(), vb.end());

    auto& attributes = m_configs.vertex_input_attribute_descriptions;
    auto av = std::views::transform(vertex_attribute_descriptions,
        [] (const VertexInputAttributeDescription& desc) {
            return VkVertexInputAttributeDescription {
                .location = desc.location,
                .binding = desc.binding,
                .format = static_cast<VkFormat>(desc.format),
                .offset = desc.offset,
            };
        }
    );
    attributes.insert(attributes.end(), av.begin(), av.end());

    auto& ias = m_current_config.input_assembly_state;
    const auto& iai = input_assembly_info;
    ias = {
        .sType = sType(ias),
        .topology = static_cast<VkPrimitiveTopology>(iai.primitive_topology),
        .primitiveRestartEnable = iai.primitive_restart_enabled,
    };
    cfg.dynamic_primitive_restart = iai.dynamic_primitive_restart;
    cfg.dynamic_primitive_topology = iai.dynamic_primitive_topology;

    return *this;
}

GPC& GPC::SetTessellationShaderState(
    const ShaderStageInfo& tesselation_control_shader_info,
    const ShaderStageInfo& tesselation_evaluation_shader_info,
    const TesselationInfo& tesselation_info
) {
    auto& cfg = m_current_config;

    auto cidx = cfg.tesselation_control_stage_idx;
    fillShaderStage(
        cfg.shader_stages[cidx],
        cfg.shader_entry_points[cidx],
        std::move(tesselation_control_shader_info),
        VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT
    );

    auto eidx = cfg.tesselation_evaluation_stage_idx;
    fillShaderStage(
        cfg.shader_stages[eidx],
        cfg.shader_entry_points[eidx],
        std::move(tesselation_evaluation_shader_info),
        VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT
    );

    auto& ts = cfg.tesselation_state;
    const auto& ti = tesselation_info;
    ts = {
        .sType = sType(ts),
        .patchControlPoints = ti.patch_control_point_count
    };

    return *this;
}

GPC& GPC::SetGeometryShaderState(
    const ShaderStageInfo& geometry_shader_info
) {
    auto& cfg = m_current_config;

    auto idx = m_current_config.geometry_stage_idx;
    fillShaderStage(
        cfg.shader_stages[idx],
        cfg.shader_entry_points[idx],
        std::move(geometry_shader_info),
        VK_SHADER_STAGE_GEOMETRY_BIT
    );

    return *this;
}

GPC& GPC::SetRasterizationState(
    const RasterizationInfo& rasterization_info,
    const MultisampleInfo& multisample_info
) {
    auto& cfg = m_current_config;

    auto& rs = cfg.rasterization_state;
    const auto& ri = rasterization_info;
    rs = {
        .sType = sType(rs),
        .polygonMode = static_cast<VkPolygonMode>(ri.polygon_mode),
        .cullMode = static_cast<VkCullModeFlags>(ri.cull_mode),
        .frontFace = static_cast<VkFrontFace>(ri.front_face),
        .depthBiasEnable = ri.depth_bias.enabled,
        .depthBiasConstantFactor = ri.depth_bias.constant_factor,
        .depthBiasClamp = ri.depth_bias.clamp,
        .depthBiasSlopeFactor = ri.depth_bias.slope_factor,
        .lineWidth = ri.line_width,
    };
    cfg.dynamic_rasterizer_discard = ri.dynamic_discard;
    cfg.dynamic_cull_mode = ri.dynamic_cull_mode;
    cfg.dynamic_front_face = ri.dynamic_front_face;
    cfg.dynamic_line_width = ri.dynamic_line_width;
    cfg.dynamic_depth_bias = ri.depth_bias.dynamic;
    cfg.dynamic_depth_bias_params = ri.depth_bias.dynamic_values;

    auto& ms = cfg.multisample_state;
    const auto& mi = multisample_info;
    ms = {
        .sType = sType(ms),
        .rasterizationSamples =
            static_cast<VkSampleCountFlagBits>(mi.sample_count),
        .sampleShadingEnable = mi.sample_shading.enabled,
        .minSampleShading = mi.sample_shading.min,
        // This must be updated in FinishAll
        .pSampleMask =
            reinterpret_cast<const uint32_t*>(&cfg.sample_mask),
        .alphaToCoverageEnable = mi.alpha_to_coverage,
        .alphaToOneEnable = mi.alpha_to_one,
    };
    if (mi.sample_mask) {
        cfg.sample_mask = *mi.sample_mask;
    }

    return *this;
}

GPC& GPC::SetDepthTestState(
    const DepthTestInfo& depth_test_info,
    const DepthAttachmentInfo& depth_attachment_info
) {
    auto& cfg = m_current_config;

    auto& dss = cfg.depth_stencil_state;
    const auto& dti = depth_test_info;
    dss.depthTestEnable = dti.enabled;
    dss.depthWriteEnable = dti.write_enabled;
    dss.depthCompareOp = static_cast<VkCompareOp>(dti.compare_op);
    dss.depthBoundsTestEnable = dti.bounds_test.enabled;
    dss.minDepthBounds = dti.bounds_test.min;
    dss.maxDepthBounds = dti.bounds_test.max;
    cfg.dynamic_depth_test = dti.dynamic;
    cfg.dynamic_depth_write = dti.dynamic_write;
    cfg.dynamic_depth_compare_op = dti.dynamic_compare_op;
    cfg.dynamic_depth_bounds_test = dti.bounds_test.dynamic;
    cfg.dynamic_depth_bounds_test_params = dti.bounds_test.dynamic_values;

    auto& rs = cfg.rasterization_state;
    rs.depthClampEnable = dti.clamp_enabled;

    auto& r = cfg.rendering;
    const auto& dai = depth_attachment_info;
    r.depthAttachmentFormat = static_cast<VkFormat>(dai.format);

    return *this;
}

GPC& GPC::SetStencilTestState(
    const StencilTestInfo& stencil_test_info,
    const StencilAttachmentInfo& stencil_attachment_info
) {
    auto& cfg = m_current_config;

    auto& dss = cfg.depth_stencil_state;
    const auto& sti = stencil_test_info;
    auto assign = [] (VkStencilOpState& vops, const auto& ops) {
        vops.failOp =
            static_cast<VkStencilOp>(ops.fail_op);
        vops.passOp =
            static_cast<VkStencilOp>(ops.pass_op);
        vops.depthFailOp =
            static_cast<VkStencilOp>(ops.depth_fail_op);
        vops.compareOp =
            static_cast<VkCompareOp>(ops.compare_op);
        vops.compareMask = ops.compare_mask;
        vops.writeMask = ops.write_mask;
        vops.reference = ops.reference;
    };
    dss.stencilTestEnable = sti.enabled;
    assign(dss.front, sti.front);
    assign(dss.back, sti.back);
    cfg.dynamic_stencil_test = sti.dynamic;
    cfg.dynamic_stencil_test_ops = sti.dynamic_op;
    cfg.dynamic_stencil_test_compare_mask = sti.dynamic_compare_mask;
    cfg.dynamic_stencil_test_write_mask = sti.dynamic_write_mask;
    cfg.dynamic_stencil_test_reference = sti.dynamic_reference;

    auto& r = cfg.rendering;
    const auto& sai = stencil_attachment_info;
    r.stencilAttachmentFormat = static_cast<VkFormat>(sai.format);

    return *this;
}

GPC& GPC::SetFragmentShaderState(
    const ShaderStageInfo& fragment_shader_info,
    const ColorBlendInfo& color_blend_info,
    std::span<const ColorAttachmentInfo> color_attachment_infos
) {
    auto& cfg = m_current_config;

    auto idx = cfg.fragment_stage_idx;
    fillShaderStage(
        cfg.shader_stages[idx],
        cfg.shader_entry_points[idx],
        std::move(fragment_shader_info),
        VK_SHADER_STAGE_FRAGMENT_BIT
    );

    auto& cbs = cfg.color_blend_state;
    const auto& cbi = color_blend_info;
    const auto& cai = color_attachment_infos;
    cbs = {
        .sType = sType(cbs),
        .logicOpEnable = cbi.logic_op_enabled,
        .logicOp = static_cast<VkLogicOp>(cbi.logic_op),
    };
    std::ranges::copy(cbi.constants, cbs.blendConstants);
    cfg.dynamic_blend_constants = cbi.dynamic_constants;

    auto& color_blend_attachments = m_configs.color_blend_attachments;
    cbs.attachmentCount = cai.size();
    auto bav = std::views::transform(cai,
        [] (const auto& ca) {
            return VkPipelineColorBlendAttachmentState {
                .blendEnable = ca.blend.enabled,
                .srcColorBlendFactor =
                    static_cast<VkBlendFactor>(ca.blend.src_color_factor),
                .dstColorBlendFactor =
                    static_cast<VkBlendFactor>(ca.blend.dst_color_factor),
                .colorBlendOp =
                    static_cast<VkBlendOp>(ca.blend.color_op),
                .srcAlphaBlendFactor =
                    static_cast<VkBlendFactor>(ca.blend.src_alpha_factor),
                .dstAlphaBlendFactor =
                    static_cast<VkBlendFactor>(ca.blend.dst_alpha_factor),
                .alphaBlendOp =
                    static_cast<VkBlendOp>(ca.blend.alpha_op),
                .colorWriteMask = ColorComponentMaskToVK(ca.color_mask),
            };
        }
    );
    color_blend_attachments.insert(
        color_blend_attachments.end(),
        bav.begin(), bav.end()
    );

    auto& r = cfg.rendering;
    auto& color_attachment_formats = m_configs.color_attachment_formats;
    r.colorAttachmentCount = cai.size();
    auto afv = std::views::transform(cai,
        [] (const auto& ca) {
            return static_cast<VkFormat>(ca.format);
        }
    );
    color_attachment_formats.insert(
        color_attachment_formats.end(),
        afv.begin(), afv.end()
    );

    return *this;
}

GPC& GPC::FinishCurrent() {
    auto& cfg = m_current_config;

    auto& dstates = m_configs.dynamic_states;
    auto start_size = dstates.size();
    dstates.insert(dstates.end(), {
        VK_DYNAMIC_STATE_VIEWPORT_WITH_COUNT,
        VK_DYNAMIC_STATE_SCISSOR_WITH_COUNT,
    });
    if (cfg.dynamic_vertex_input_binding_stride) {
        dstates.push_back(VK_DYNAMIC_STATE_VERTEX_INPUT_BINDING_STRIDE);
    }
    if (cfg.dynamic_primitive_topology) {
        dstates.push_back(VK_DYNAMIC_STATE_PRIMITIVE_TOPOLOGY);
    }
    if (cfg.dynamic_primitive_restart) {
        dstates.push_back(VK_DYNAMIC_STATE_PRIMITIVE_RESTART_ENABLE);
    }
    if (cfg.dynamic_rasterizer_discard) {
        dstates.push_back(VK_DYNAMIC_STATE_RASTERIZER_DISCARD_ENABLE);
    }
    if (cfg.dynamic_cull_mode) {
        dstates.push_back(VK_DYNAMIC_STATE_CULL_MODE);
    } 
    if (cfg.dynamic_front_face) {
        dstates.push_back(VK_DYNAMIC_STATE_FRONT_FACE);
    }
    if (cfg.dynamic_line_width) {
        dstates.push_back(VK_DYNAMIC_STATE_LINE_WIDTH);
    }
    if (cfg.dynamic_depth_bias) {
        dstates.push_back(VK_DYNAMIC_STATE_DEPTH_BIAS_ENABLE);
    }
    if (cfg.dynamic_depth_bias_params) {
        dstates.push_back(VK_DYNAMIC_STATE_DEPTH_BIAS);
    }
    if (cfg.dynamic_depth_test) {
        dstates.push_back(VK_DYNAMIC_STATE_DEPTH_TEST_ENABLE);
    }
    if (cfg.dynamic_depth_write) {
        dstates.push_back(VK_DYNAMIC_STATE_DEPTH_WRITE_ENABLE);
    }
    if (cfg.dynamic_depth_compare_op) {
        dstates.push_back(VK_DYNAMIC_STATE_DEPTH_COMPARE_OP);
    }
    if (cfg.dynamic_depth_bounds_test) {
        dstates.push_back(VK_DYNAMIC_STATE_DEPTH_BOUNDS_TEST_ENABLE);
    }
    if (cfg.dynamic_depth_bounds_test_params) {
        dstates.push_back(VK_DYNAMIC_STATE_DEPTH_BOUNDS);
    }
    if (cfg.dynamic_stencil_test) {
        dstates.push_back(VK_DYNAMIC_STATE_STENCIL_TEST_ENABLE);
    }
    if (cfg.dynamic_stencil_test_ops) {
        dstates.push_back(VK_DYNAMIC_STATE_STENCIL_OP);
    }
    if (cfg.dynamic_stencil_test_compare_mask) {
        dstates.push_back(VK_DYNAMIC_STATE_STENCIL_COMPARE_MASK);
    }
    if (cfg.dynamic_stencil_test_write_mask) {
        dstates.push_back(VK_DYNAMIC_STATE_STENCIL_WRITE_MASK);
    }
    if (cfg.dynamic_stencil_test_reference) {
        dstates.push_back(VK_DYNAMIC_STATE_STENCIL_REFERENCE);
    }
    if (cfg.dynamic_blend_constants) {
        dstates.push_back(VK_DYNAMIC_STATE_BLEND_CONSTANTS);
    }
    auto end_size = dstates.size();

    cfg.rendering.sType = sType(cfg.rendering);
    cfg.viewport_state.sType = sType(cfg.viewport_state);
    cfg.rasterization_state.sType = sType(cfg.rasterization_state);
    cfg.depth_stencil_state.sType = sType(cfg.depth_stencil_state);
    cfg.dynamic_state = {
        .sType = sType(cfg.dynamic_state),
        .dynamicStateCount =
            static_cast<uint32_t>(end_size - start_size),
    };

    m_configs.configs.emplace_back(std::exchange(m_current_config, {}));
    m_configs.create_infos.emplace_back(std::exchange(m_current_create_info, {}));

    return *this;
}

GraphicsPipelineConfigs GPC::FinishAll() {
    auto binding_p =
        m_configs.vertex_input_binding_descriptions.data();
    auto attribute_p =
        m_configs.vertex_input_attribute_descriptions.data();
    auto color_blend_attachment_p =
        m_configs.color_blend_attachments.data();
    auto color_attachment_format_p =
        m_configs.color_attachment_formats.data();
    auto dynamic_state_p =
        m_configs.dynamic_states.data();

    for (size_t i = 0; i < m_configs.configs.size(); i++) {
        auto& cfg = m_configs.configs[i];
        auto& create_info = m_configs.create_infos[i];

        for (size_t s = 0; s < cfg.stage_count; s++) {
            cfg.shader_stages[s].pName =
                cfg.shader_entry_points[s].c_str();
        }
        auto p = std::ranges::partition(cfg.shader_stages,
            [] (const auto& stage) {
                return stage.module == VK_NULL_HANDLE;
            }
        );
        create_info.stageCount = p.size();
        create_info.pStages = p.data();

        auto& vis = cfg.vertex_input_state;
        vis.pVertexBindingDescriptions = binding_p;
        binding_p += vis.vertexBindingDescriptionCount;
        vis.pVertexAttributeDescriptions = attribute_p;
        attribute_p += vis.vertexAttributeDescriptionCount;

        auto& ms = cfg.multisample_state;
        if (ms.pSampleMask) {
            ms.pSampleMask =
                reinterpret_cast<const uint32_t*>(&cfg.sample_mask);
        }

        auto& cbs = cfg.color_blend_state;
        cbs.pAttachments = color_blend_attachment_p;
        color_blend_attachment_p += cbs.attachmentCount;

        auto& r = cfg.rendering;
        r.pColorAttachmentFormats = color_attachment_format_p;
        color_attachment_format_p += r.colorAttachmentCount;

        auto& ds = cfg.dynamic_state;
        ds.pDynamicStates = dynamic_state_p;
        dynamic_state_p += ds.dynamicStateCount;

        create_info.sType = sType(create_info);
        create_info.pNext = &cfg.rendering;
        create_info.pVertexInputState = &cfg.vertex_input_state;
        create_info.pInputAssemblyState = &cfg.input_assembly_state;
        create_info.pTessellationState = &cfg.tesselation_state;
        create_info.pViewportState = &cfg.viewport_state;
        create_info.pRasterizationState = &cfg.rasterization_state;
        create_info.pMultisampleState = &cfg.multisample_state;
        create_info.pDepthStencilState = &cfg.depth_stencil_state;
        create_info.pColorBlendState = &cfg.color_blend_state;
        create_info.pDynamicState = &cfg.dynamic_state;
    }

    return std::exchange(m_configs, {});
}

void CreateGraphicsPipelines(
    Context ctx, PipelineCache pipeline_cache,
    const GraphicsPipelineConfigs& configs,
    Pipeline* out
) {
    auto cnt = configs.create_infos.size();
    auto r = vkCreateGraphicsPipelines(
        ctx->device.get(), pipeline_cache,
        cnt, configs.create_infos.data(),
        nullptr,
        out
    );
    if (r != VK_SUCCESS) {
        throw std::runtime_error{"Vulkan: Failed to create pipelines"};
    }
}
}
