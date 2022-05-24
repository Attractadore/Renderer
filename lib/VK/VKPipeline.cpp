#include "VKContext.hpp"

#include <algorithm>
#include <ranges>
#include <utility>

namespace R1::VK {
namespace {
void fillShaderStage(
    VkPipelineShaderStageCreateInfo& stage, std::string& entry_point,
    const ShaderStageInfo& stage_info, VkShaderStageFlagBits type
) {
    entry_point = stage_info.entry_point;
    stage = {
        .sType = sType(stage),
        .stage = type,
        .module = stage_info.module.module,
    };
}
}

using GPC = GraphicsPipelineConfigurator;

GPC& GPC::SetLayout(PipelineLayout layout) {
    m_current_create_info.layout = layout.layout;
    return *this;
}

GPC& GPC::SetVertexShaderState(
    const ShaderStageInfo& vertex_shader_info,
    const VertexInputInfo& vertex_input_info,
    std::initializer_list<VertexInputBindingDescription>
        vertex_input_binding_descriptions,
    std::initializer_list<VertexInputAttributeDescription>
        vertex_input_attribute_descriptions,
    const InputAssemblyInfo& input_assembly_info
) {
    auto& cfg = m_current_config;

    auto idx = cfg.vertex_stage_idx;
    const auto& vsi = vertex_shader_info;
    fillShaderStage(
        cfg.shader_stages[idx],
        cfg.shader_entry_points[idx],
        vsi, VK_SHADER_STAGE_VERTEX_BIT 
    );

    auto& vis = cfg.vertex_input_state;
    const auto& vii = vertex_input_info;
    vis = {
        .sType = sType(vis),
    };
    cfg.dynamic_vertex_input_binding_stride =
        vii.dynamic_stride;

    auto& bindings = m_configs.vertex_input_binding_descriptions;
    bindings.resize(bindings.size() - vis.vertexBindingDescriptionCount);
    vis.vertexBindingDescriptionCount = 
        vertex_input_binding_descriptions.size();
    auto vb = std::views::transform(vertex_input_binding_descriptions,
        [] (const VertexInputBindingDescription& desc) {
            VkVertexInputBindingDescription vdesc = {
                .binding = desc.binding,
                .stride = desc.stride,
                .inputRate = static_cast<VkVertexInputRate>(desc.input_rate),
            };
            return vdesc;
        }
    );
    bindings.insert(bindings.end(), vb.begin(), vb.end());

    auto& attributes = m_configs.vertex_input_attribute_descriptions;
    attributes.resize(attributes.size() - vis.vertexAttributeDescriptionCount); 
    vis.vertexAttributeDescriptionCount = 
        vertex_input_attribute_descriptions.size();
    auto av = std::views::transform(vertex_input_attribute_descriptions,
        [] (const VertexInputAttributeDescription& desc) {
            VkVertexInputAttributeDescription vdesc = {
                .location = desc.location,
                .binding = desc.binding,
                .format = static_cast<VkFormat>(desc.format),
                .offset = desc.offset,
            };
            return vdesc;
        }
    );
    attributes.insert(attributes.end(), av.begin(), av.end());

    auto& ias = m_current_config.input_assembly_state;
    const auto& iai = input_assembly_info;
    ias = {
        .sType = sType(ias),
    };
    cfg.dynamic_primitive_restart =
        iai.primitive_restart_enabled.isDynamic();
    std::visit([&] <typename T> (const T& pt) {
        ias.topology = static_cast<VkPrimitiveTopology>(pt);
        cfg.dynamic_primitive_topology =
            std::same_as<T, PrimitiveTopologyClass>;
    }, iai.primitive_topology);
    iai.primitive_restart_enabled.visit_if(
        [&] (bool pr) { ias.primitiveRestartEnable = pr; }
    );

    return *this;
}

GPC& GPC::SetTessellationShaderState(
    const ShaderStageInfo& tesselation_control_shader_info,
    const ShaderStageInfo& tesselation_evaluation_shader_info,
    const TesselationInfo& tesselation_info
) {
    auto& cfg = m_current_config;

    auto cidx = cfg.tesselation_control_stage_idx;
    const auto& tcsi = tesselation_control_shader_info;
    fillShaderStage(
        cfg.shader_stages[cidx],
        cfg.shader_entry_points[cidx],
        tcsi, VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT 
    );

    auto eidx = cfg.tesselation_evaluation_stage_idx;
    const auto& tesi = tesselation_evaluation_shader_info;
    fillShaderStage(
        cfg.shader_stages[eidx],
        cfg.shader_entry_points[eidx],
        tesi, VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT 
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
    const auto& gsi = geometry_shader_info;
    fillShaderStage(
        cfg.shader_stages[idx],
        cfg.shader_entry_points[idx],
        gsi, VK_SHADER_STAGE_GEOMETRY_BIT
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
    };
    cfg.dynamic_rasterizer_discard = ri.dynamic_discard;
    cfg.dynamic_cull_mode = ri.cull_mode.isDynamic();
    cfg.dynamic_front_face = ri.front_face.isDynamic();
    cfg.dynamic_line_width = ri.line_width.isDynamic();
    ri.cull_mode.visit_if(
        [&] (CullMode cm) { rs.cullMode = static_cast<VkCullModeFlags>(cm); }
    );
    ri.front_face.visit_if(
        [&] (FrontFace ff) { rs.frontFace = static_cast<VkFrontFace>(ff); }
    );
    if (ri.depth_bias) {
        rs.depthBiasEnable = true;
        cfg.dynamic_depth_bias = ri.depth_bias->dynamic;
        cfg.dynamic_depth_bias_params =
            ri.depth_bias->params.isDynamic();
        ri.depth_bias->params.visit_if(
            [&] (const auto& db) {
                rs.depthBiasConstantFactor = db.constant_factor;
                rs.depthBiasClamp = db.clamp;
                rs.depthBiasSlopeFactor = db.slope_factor;
            }
        );
    }
    ri.line_width.visit_if(
        [&] (float lw) { rs.lineWidth = lw; }
    );

    auto& ms = cfg.multisample_state;
    const auto& mi = multisample_info;
    ms = {
        .sType = sType(ms),
        .rasterizationSamples =
            static_cast<VkSampleCountFlagBits>(mi.sample_count),
        .alphaToCoverageEnable = mi.alpha_to_coverage,
        .alphaToOneEnable = mi.alpha_to_one,
    };
    if (mi.sample_shading) {
        ms.sampleShadingEnable = true;
        ms.minSampleShading = mi.sample_shading->min;
    }
    if (mi.sample_mask) {
        cfg.sample_mask = *mi.sample_mask;
        // This must be updated in FinishAll
        ms.pSampleMask =
            reinterpret_cast<const uint32_t*>(&cfg.sample_mask);
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
    dss.depthTestEnable = true;
    cfg.dynamic_depth_test = dti.dynamic;
    cfg.dynamic_depth_write = dti.write.isDynamic();
    cfg.dynamic_depth_compare_op = dti.compare_op.isDynamic();
    dti.write.visit_if(
        [&] (bool dw) { dss.depthWriteEnable = dw; }
    );
    dti.compare_op.visit_if(
        [&] (CompareOp cmp) { dss.depthCompareOp = static_cast<VkCompareOp>(cmp); }
    );
    if (dti.bounds_test) {
        dss.depthBoundsTestEnable = true;
        cfg.dynamic_depth_bounds_test = dti.bounds_test->dynamic;
        cfg.dynamic_depth_bounds_test_params =
            dti.bounds_test->params.isDynamic();
        dti.bounds_test->params.visit_if(
            [&] (const auto& db) {
                dss.minDepthBounds = db.min;
                dss.maxDepthBounds = db.max;
            }
        );
    }

    auto& rs = cfg.rasterization_state;
    rs.depthClampEnable = dti.clamp;

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
    dss.stencilTestEnable = true;
    cfg.dynamic_stencil_test = sti.dynamic;
    cfg.dynamic_stencil_test_ops = sti.ops.isDynamic();
    cfg.dynamic_stencil_test_compare_mask = sti.compare_mask.isDynamic();
    cfg.dynamic_stencil_test_write_mask = sti.write_mask.isDynamic();
    cfg.dynamic_stencil_test_reference = sti.reference.isDynamic();
    sti.ops.visit_if(
        [&] (const auto& ops) {
            auto assign = [] (VkStencilOpState vops, const auto& ops) {
                vops.failOp =
                    static_cast<VkStencilOp>(ops.fail);
                vops.passOp =
                    static_cast<VkStencilOp>(ops.pass);
                vops.depthFailOp =
                    static_cast<VkStencilOp>(ops.depth_fail);
                vops.compareOp =
                    static_cast<VkCompareOp>(ops.compare);
            };
            assign(dss.front, ops.front);
            assign(dss.back, ops.back);
        }
    );
    sti.compare_mask.visit_if(
        [&] (const auto& msk) {
            dss.front.compareMask = msk.front;
            dss.back.compareMask = msk.back;
        }
    );
    sti.write_mask.visit_if(
        [&] (const auto& msk) {
            dss.front.writeMask = msk.front;
            dss.back.writeMask = msk.back;
        }
    );
    sti.reference.visit_if(
        [&] (const auto& msk) {
            dss.front.reference = msk.front;
            dss.back.reference = msk.back;
        }
    );

    auto& r = cfg.rendering;
    const auto& sai = stencil_attachment_info;
    r.stencilAttachmentFormat = static_cast<VkFormat>(sai.format);

    return *this;
}

GPC& GPC::SetFragmentShaderState(
    const ShaderStageInfo& fragment_shader_info,
    const ColorBlendInfo& color_blend_info,
    std::initializer_list<ColorAttachmentInfo>
        color_attachment_infos
) {
    auto& cfg = m_current_config;

    auto idx = cfg.fragment_stage_idx;
    fillShaderStage(
        cfg.shader_stages[idx],
        cfg.shader_entry_points[idx],
        fragment_shader_info, VK_SHADER_STAGE_FRAGMENT_BIT
    );

    auto& cbs = cfg.color_blend_state;
    const auto& cbi = color_blend_info;
    const auto& cai = color_attachment_infos;
    cbs = {
        .sType = sType(cbs),
    };
    if (cbi.logic_op) {
        cbs.logicOpEnable = true;
        cbs.logicOp = static_cast<VkLogicOp>(*cbi.logic_op);
    }
    if (cbi.constants) {
        cfg.dynamic_blend_constants = cbi.constants->isDynamic();
        cbi.constants->visit_if(
            [&] (const auto& consts) {
                std::ranges::copy(consts, cbs.blendConstants);
            }
        );
    }

    auto& color_blend_attachments = m_configs.color_blend_attachments;
    color_blend_attachments.resize(color_blend_attachments.size() - cbs.attachmentCount);
    cbs.attachmentCount = cai.size();
    auto bav = std::views::transform(cai,
        [] (const auto& ca) {
            VkPipelineColorBlendAttachmentState vkca = {
                .colorWriteMask = (ca.color_mask.r ? VK_COLOR_COMPONENT_R_BIT: 0u) |
                                  (ca.color_mask.g ? VK_COLOR_COMPONENT_G_BIT: 0u) |
                                  (ca.color_mask.b ? VK_COLOR_COMPONENT_B_BIT: 0u) |
                                  (ca.color_mask.a ? VK_COLOR_COMPONENT_A_BIT: 0u)
            };
            if (ca.blend) {
                vkca.blendEnable = true;
                vkca.srcColorBlendFactor =
                    static_cast<VkBlendFactor>(ca.blend->src_color_factor);
                vkca.dstColorBlendFactor =
                    static_cast<VkBlendFactor>(ca.blend->dst_color_factor);
                vkca.colorBlendOp =
                    static_cast<VkBlendOp>(ca.blend->color_op);
                vkca.srcAlphaBlendFactor =
                    static_cast<VkBlendFactor>(ca.blend->src_alpha_factor);
                vkca.dstAlphaBlendFactor =
                    static_cast<VkBlendFactor>(ca.blend->dst_alpha_factor);
                vkca.alphaBlendOp =
                    static_cast<VkBlendOp>(ca.blend->alpha_op);
            }
            return vkca;
        }
    );
    color_blend_attachments.insert(
        color_blend_attachments.end(),
        bav.begin(), bav.end()
    );

    auto& r = cfg.rendering;
    auto& color_attachment_formats = m_configs.color_attachment_formats;
    color_attachment_formats.resize(color_attachment_formats.size() - r.colorAttachmentCount);
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

    bool vert = true;
    cfg.dynamic_vertex_input_binding_stride &= vert;
    cfg.dynamic_primitive_topology &= vert;
    cfg.dynamic_primitive_restart &= vert;
    bool rast = not cfg.rasterization_state.rasterizerDiscardEnable;
    cfg.dynamic_rasterizer_discard &= rast;
    cfg.dynamic_cull_mode &= rast; 
    cfg.dynamic_front_face &= rast;
    cfg.dynamic_line_width &= rast;
    bool depth_bias = rast and cfg.rasterization_state.depthBiasEnable;
    cfg.dynamic_depth_bias &= depth_bias;
    cfg.dynamic_depth_bias_params &= depth_bias;
    bool depth_test = rast and cfg.depth_stencil_state.depthTestEnable;
    cfg.dynamic_depth_test &= depth_test;
    cfg.dynamic_depth_write &= depth_test;
    cfg.dynamic_depth_compare_op &= depth_test;
    bool depth_bounds_test = depth_test and cfg.depth_stencil_state.depthBoundsTestEnable;
    cfg.dynamic_depth_bounds_test &= depth_bounds_test;
    cfg.dynamic_depth_bounds_test_params &= depth_bounds_test;
    bool stencil_test = rast and cfg.depth_stencil_state.stencilTestEnable;
    cfg.dynamic_stencil_test &= stencil_test;
    cfg.dynamic_stencil_test_ops &= stencil_test;
    cfg.dynamic_stencil_test_compare_mask &= stencil_test;
    cfg.dynamic_stencil_test_write_mask &= stencil_test;
    cfg.dynamic_stencil_test_reference &= stencil_test;
    bool blend = rast;
    cfg.dynamic_blend_constants &= blend;

    auto& ds = cfg.dynamic_state;
    ds = {
        .sType = sType(ds),
    };

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
    cfg.dynamic_state.dynamicStateCount = end_size - start_size;

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

std::vector<Pipeline> Context::CreateGraphicsPipelines(
    const GraphicsPipelineConfigs& pipeline_configs
) {
    auto cnt = pipeline_configs.create_infos.size();
    std::vector<Pipeline> pipelines(cnt);
    auto r = vkCreateGraphicsPipelines(
        m_device.get(),
        VK_NULL_HANDLE,
        cnt,
        pipeline_configs.create_infos.data(), 
        nullptr,
        reinterpret_cast<VkPipeline*>(pipelines.data())
    );
    if (r != VK_SUCCESS) {
        throw std::runtime_error{"Vulkan: Failed to create pipelines"};
    }
    return pipelines;
}
}
