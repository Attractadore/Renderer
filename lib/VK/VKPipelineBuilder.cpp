#include "VKPipelineBuilder.hpp"
#include "VKUtil.hpp"

#include <algorithm>
#include <bit>
#include <cassert>
#include <ranges>

namespace R1::VK {
using VKPSSCI = VkPipelineShaderStageCreateInfo;
VKPSSCI& Detail::ShaderStages::vertex() {
    m_vertex_stage = index(m_vertex_stage);
    return Base::at(m_vertex_stage);
}

const VKPSSCI& Detail::ShaderStages::vertex() const {
    assert(hasVertex());
    return Base::at(m_vertex_stage);
}

bool Detail::ShaderStages::hasVertex() const {
    return hasStage(m_vertex_stage);
}

VKPSSCI& Detail::ShaderStages::fragment() {
    m_fragment_stage = index(m_fragment_stage);
    return Base::at(m_fragment_stage);
}

const VKPSSCI& Detail::ShaderStages::fragment() const {
    assert(hasFragment());
    return Base::at(m_fragment_stage);
}

bool Detail::ShaderStages::hasFragment() const {
    return hasStage(m_fragment_stage);
}

bool Detail::ShaderStages::hasTesselation() const {
    return false;
}

unsigned Detail::ShaderStages::index(unsigned idx) {
    if (idx == c_stage_not_active) {
        idx = m_stage_count++;
    }
    return idx;
};

bool Detail::ShaderStages::hasStage(unsigned idx) {
    return idx != c_stage_not_active;
}

ColorAttachments::ColorAttachments(std::initializer_list<ColorAttachment> list):
m_blend { [&] {
    auto color_attachment_to_vk_blend_state = [] (const auto& color_attachment) {
        VkPipelineColorBlendAttachmentState cbas = {};
        const auto& blend_state = color_attachment.blend_state;
        const auto& color_mask  = color_attachment.color_mask;
        if (blend_state.has_value()) {
            auto& v = blend_state.value();
            cbas.blendEnable            = true;
            cbas.srcColorBlendFactor    = static_cast<VkBlendFactor>(v.src_color_blend_factor);
            cbas.dstColorBlendFactor    = static_cast<VkBlendFactor>(v.dst_color_blend_factor);
            cbas.colorBlendOp           = static_cast<VkBlendOp>(v.color_blend_op);
            cbas.srcAlphaBlendFactor    = static_cast<VkBlendFactor>(v.src_alpha_blend_factor);
            cbas.dstAlphaBlendFactor    = static_cast<VkBlendFactor>(v.dst_alpha_blend_factor);
            cbas.alphaBlendOp           = static_cast<VkBlendOp>(v.alpha_blend_op);
        }
        cbas.colorWriteMask =
            (color_mask.r ? VK_COLOR_COMPONENT_R_BIT : 0) |
            (color_mask.b ? VK_COLOR_COMPONENT_G_BIT : 0) |
            (color_mask.g ? VK_COLOR_COMPONENT_B_BIT : 0) |
            (color_mask.a ? VK_COLOR_COMPONENT_A_BIT : 0);
        return cbas;
    };
    auto v = std::views::transform(list, color_attachment_to_vk_blend_state);
    return ColorBlendAttachments(v.begin(), v.end());
} () },
m_format { [&] {
    auto color_attachment_to_vk_format = [] (const auto& color_attachment) {
        return static_cast<VkFormat>(color_attachment.format);
    };
    auto v = std::views::transform(list, color_attachment_to_vk_format);
    return ColorAttachmentFormats(v.begin(), v.end());
} () } {}

using GPB = GraphicsPipelineBuilder;
GPB::GraphicsPipelineBuilder(VkDevice dev):
    m_device{dev} {}

GPB& GPB::setLayout(const PipelineLayout& layout) {
    m_layout = layout;
    return *this;
}

namespace {
VkPipelineShaderStageCreateInfo fillShaderStage(
    VkShaderStageFlagBits stage,
    VkShaderModule module,
    const char* entry_point
) {
    VkPipelineShaderStageCreateInfo create_info {
        .sType = sType(create_info),
        .stage = stage,
        .module = module,
        .pName = entry_point,
    };
    return create_info;
}
}

GPB& GPB::setVertexStage(const ShaderModule& shader_module, const char* entry_point) {
    m_stages.vertex() = fillShaderStage(
        VK_SHADER_STAGE_VERTEX_BIT,
        shader_module.module, entry_point
    );
    return *this;
}

GPB& GPB::setFragmentStage(const ShaderModule& shader_module, const char* entry_point) {
    m_stages.fragment() = fillShaderStage(
        VK_SHADER_STAGE_FRAGMENT_BIT,
        shader_module.module, entry_point
    );
    return *this;
}

GPB& GPB::setVertexInputState(VertexInputState vertex_input_state) {
    m_vertex_input_state = std::move(vertex_input_state);
    return *this;
}

GPB& GPB::setInputAssemblyState(const InputAssemblyState& input_assembly_state) {
    m_input_assembly_state = input_assembly_state;
    return *this;
}

GPB& GPB::setTessellationState(const TessellationState& tesselation_state) {
    m_tesselation_state = tesselation_state;
    return *this;
}

GPB& GPB::setViewportState(ViewportState viewport_state) {
    m_viewport_state = std::move(viewport_state);
    return *this;
}

GPB& GPB::setRasterizationState(const RasterizationState& rasterization_state) {
    m_rasterization_state = rasterization_state;
    return *this;
}

GPB& GPB::setMultisampleState(const MultisampleState& multisample_state) {
    m_multisample_state = multisample_state;
    return *this;
}

GPB& GPB::setDepthTestState(const DepthTestState& depth_test_state) {
    m_depth_test_state = depth_test_state;
    return *this;
}

GPB& GPB::setStencilTestState(const StencilTestState& stencil_test_state) {
    m_stencil_test_state = stencil_test_state;
    return *this;
}

GPB& GPB::setColorState(ColorState color_state) {
    m_color_state = std::move(color_state);
    return *this;
}

Pipeline GPB::build() {
    verifyAndNormalize();

    std::vector<VkDynamicState> dynamic_state;

    VkPipelineRenderingCreateInfo rendering_info = {
        .sType = sType(rendering_info),
    };
    VkPipelineVertexInputStateCreateInfo vertex_input_info = {
        .sType = sType(vertex_input_info),
    };
    VkPipelineInputAssemblyStateCreateInfo input_assembly_info = {
        .sType = sType(input_assembly_info),
    };
    VkPipelineTessellationStateCreateInfo tesselation_info = {
        .sType = sType(tesselation_info),
    };
    VkPipelineViewportStateCreateInfo viewport_info = {
        .sType = sType(viewport_info),
        .viewportCount =
            static_cast<uint32_t>(m_viewport_state.viewports.size()),
        .pViewports = m_viewport_state.viewports.data(),
        .scissorCount =
            static_cast<uint32_t>(m_viewport_state.scissors.size()),
        .pScissors = m_viewport_state.scissors.data(),
    };
    if (m_viewport_state.viewports.size() == 0) {
        dynamic_state.push_back(VK_DYNAMIC_STATE_VIEWPORT_WITH_COUNT);
    }
    if (m_viewport_state.scissors.size() == 0) {
        dynamic_state.push_back(VK_DYNAMIC_STATE_SCISSOR_WITH_COUNT);
    }
    VkPipelineRasterizationStateCreateInfo rasterization_info = {
        .sType = sType(rasterization_info),
    };
    VkPipelineMultisampleStateCreateInfo multisample_info = {
        .sType = sType(multisample_info),
        .rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
    };
    VkPipelineDepthStencilStateCreateInfo depth_stencil_info = {
        .sType = sType(depth_stencil_info),
    };
    VkPipelineColorBlendStateCreateInfo color_blend_info = {
        .sType = sType(color_blend_info),
    };

    if (const auto& vis = m_vertex_input_state) {
        auto& vii = vertex_input_info;
        vii.vertexBindingDescriptionCount   = vis->bindings.size();
        vii.pVertexBindingDescriptions      = vis->bindings.data();
        vii.vertexAttributeDescriptionCount = vis->attributes.size();
        vii.pVertexAttributeDescriptions    = vis->attributes.data();
        bool dyn_stride =
            not vis->bindings.empty() and
            vis->bindings[0].stride == VertexInputBinding::StrideDynamic;
        if (dyn_stride) {
            dynamic_state.push_back(VK_DYNAMIC_STATE_VERTEX_INPUT_BINDING_STRIDE);
        }
    }

    if (const auto& ias = m_input_assembly_state) {
        auto& iai = input_assembly_info;
        std::visit([&] <typename T> (const T& topology) {
            iai.topology = static_cast<VkPrimitiveTopology>(topology);
            if constexpr (std::same_as<T, PrimitiveTopologyClass>) {
                dynamic_state.push_back(VK_DYNAMIC_STATE_PRIMITIVE_TOPOLOGY);
            }
        }, ias->topology);
        ias->restart.map(
            [&] (bool pr)   { iai.primitiveRestartEnable = pr; },
            [&]             { dynamic_state.push_back(VK_DYNAMIC_STATE_PRIMITIVE_RESTART_ENABLE); }
        );
    }

    if (const auto& ts = m_tesselation_state) {
        tesselation_info.patchControlPoints = ts->patch_control_point_count;
    }

    if (const auto& rs = m_rasterization_state) {
        auto& ri = rasterization_info;
        if (rs->dynamic_discard) {
            dynamic_state.push_back(VK_DYNAMIC_STATE_RASTERIZER_DISCARD_ENABLE);
        }
        ri.polygonMode = static_cast<VkPolygonMode>(rs->polygon_mode);
        rs->cull_mode.map(
            [&] (CullMode cm)   { ri.cullMode = static_cast<VkCullModeFlagBits>(cm); },
            [&]                 { dynamic_state.push_back(VK_DYNAMIC_STATE_CULL_MODE); }
        );
        rs->front_face.map(
            [&] (FrontFace ff)  { ri.frontFace = static_cast<VkFrontFace>(ff); },
            [&]                 { dynamic_state.push_back(VK_DYNAMIC_STATE_FRONT_FACE); }
        );
        rs->line_width.map(
            [&] (float lw)      { ri.lineWidth = lw; },
            [&]                 { dynamic_state.push_back(VK_DYNAMIC_STATE_LINE_WIDTH); }
        );
        if (const auto& db = rs->depth_bias) {
            ri.depthBiasEnable = true;
            if (db->dynamic) {
                dynamic_state.push_back(VK_DYNAMIC_STATE_DEPTH_BIAS_ENABLE);
            }
            db->values.map(
                [&] (const DepthBiasState::Values& vs) {
                    ri.depthBiasConstantFactor  = vs.constant_factor;
                    ri.depthBiasClamp           = vs.clamp;
                    ri.depthBiasSlopeFactor     = vs.slope_factor;
                },
                [&] { dynamic_state.push_back(VK_DYNAMIC_STATE_DEPTH_BIAS); }
            );
        }
    }
    else {
        rasterization_info.rasterizerDiscardEnable = true;
    }

    if (const auto& mss = m_multisample_state) {
        auto& msi = multisample_info;
        msi.rasterizationSamples =
            static_cast<VkSampleCountFlagBits>(mss->sample_count);
        if (mss->sample_shading) {
            msi.sampleShadingEnable = true;
            msi.minSampleShading = mss->sample_shading->min;
        }
        if (mss->sample_mask) {
            msi.pSampleMask =
                reinterpret_cast<const uint32_t*>(&*mss->sample_mask);
        }
        if (mss->alpha_to_coverage) {
            msi.alphaToCoverageEnable = true;
        }
        if (mss->alpha_to_one) {
            msi.alphaToOneEnable = true;
        }
    }

    if (const auto& dts = m_depth_test_state) {
        if (dts->clamp) {
            rasterization_info.depthClampEnable = true;
        }

        auto& dsi = depth_stencil_info;
        dsi.depthTestEnable = true;
        if (dts->dynamic) {
            dynamic_state.push_back(VK_DYNAMIC_STATE_DEPTH_TEST_ENABLE);
        }
        dts->write.map(
            [&] (bool dw)       { dsi.depthWriteEnable = dw; },
            [&]                 { dynamic_state.push_back(VK_DYNAMIC_STATE_DEPTH_WRITE_ENABLE); }
        );
        dts->compare_op.map(
            [&] (CompareOp op)  { dsi.depthCompareOp = static_cast<VkCompareOp>(op); },
            [&]                 { dynamic_state.push_back(VK_DYNAMIC_STATE_DEPTH_COMPARE_OP); }
        );
        if (const auto& bt = dts->bounds_test) {
            dsi.depthBoundsTestEnable = true;
            if (bt->dynamic) {
                dynamic_state.push_back(VK_DYNAMIC_STATE_DEPTH_BOUNDS_TEST_ENABLE);
            }
            bt->bounds.map(
                [&] (const DepthBoundsTestState::Bounds& bs) {
                    dsi.minDepthBounds = bs.min;
                    dsi.maxDepthBounds = bs.max;
                },
                [&] { dynamic_state.push_back(VK_DYNAMIC_STATE_DEPTH_BOUNDS); }
            );
        }

        rendering_info.depthAttachmentFormat =
            static_cast<VkFormat>(dts->attachment_format);
    }

    if (const auto& sts = m_stencil_test_state) {
        auto& dsi = depth_stencil_info;

        dsi.stencilTestEnable = true;
        if (sts->dynamic) {
            dynamic_state.push_back(VK_DYNAMIC_STATE_STENCIL_TEST_ENABLE);
        }

        bool dyn_st_op      = false;
        bool dyn_cmp_mask   = false;
        bool dyn_write_mask = false;
        bool dyn_ref        = false;
        auto lst = {
            std::tuple{&dsi.front, &sts->front},
            std::tuple{&dsi.back,  &sts->back},
        };
        for (auto [dsisosp, sosp]: lst) {
            auto& dsisos    = *dsisosp;
            auto& sos       = *sosp;
            sos.ops.map(
                [&] (const StencilOpState::Ops& ops) {
                    dsisos.failOp        = static_cast<VkStencilOp>(ops.fail);
                    dsisos.passOp        = static_cast<VkStencilOp>(ops.pass);
                    dsisos.depthFailOp   = static_cast<VkStencilOp>(ops.depth_fail);
                    dsisos.compareOp     = static_cast<VkCompareOp>(ops.compare);
                },
                [&] { dyn_st_op = true; }
            );
            sos.compare_mask.map(
                [&] (uint32_t cmp_mask) { dsisos.compareMask = cmp_mask; },
                [&] { dyn_cmp_mask = true; }
            );
            sos.write_mask.map(
                [&] (uint32_t write_mask) { dsisos.writeMask = write_mask; },
                [&] { dyn_write_mask = true; }
            );
            sos.reference.map(
                [&] (unsigned ref)  { dsisos.reference = ref; },
                [&] { dyn_ref = true; }
            );
        }
        if (dyn_st_op) {
            dynamic_state.push_back(VK_DYNAMIC_STATE_STENCIL_OP);
        }
        if (dyn_cmp_mask) {
            dynamic_state.push_back(VK_DYNAMIC_STATE_STENCIL_COMPARE_MASK);
        }
        if (dyn_write_mask) {
            dynamic_state.push_back(VK_DYNAMIC_STATE_STENCIL_WRITE_MASK);
        }
        if (dyn_ref) {
            dynamic_state.push_back(VK_DYNAMIC_STATE_STENCIL_REFERENCE);
        }

        rendering_info.stencilAttachmentFormat =
            static_cast<VkFormat>(sts->attachment_format);
    }

    if (const auto& cs = m_color_state) {
        auto& cbi = color_blend_info;
        if (cs->blend_logic_op) {
            color_blend_info.logicOpEnable = true;
            color_blend_info.logicOp =
                static_cast<VkLogicOp>(*cs->blend_logic_op);
        }
        const auto& blend_attachments = cs->attachments.blend();
        color_blend_info.attachmentCount = blend_attachments.size();
        color_blend_info.pAttachments = blend_attachments.data();
        if (auto& bc = cs->blend_constants) {
            bc->map(
                [&] (const std::array<float, 4>& c) {
                    std::ranges::copy(c, cbi.blendConstants);
                },
                [&] { dynamic_state.push_back(VK_DYNAMIC_STATE_BLEND_CONSTANTS); }
            );
        }

        auto& ri = rendering_info;
        const auto& color_fmts = cs->attachments.format();
        ri.colorAttachmentCount = color_fmts.size();
        ri.pColorAttachmentFormats = color_fmts.data();
    }

    VkPipelineDynamicStateCreateInfo dynamic_info = {
        .sType = sType(dynamic_info),
        .dynamicStateCount =
            static_cast<uint32_t>(dynamic_state.size()),
        .pDynamicStates = dynamic_state.data(),
    };

    VkGraphicsPipelineCreateInfo create_info = {
        .sType = sType(create_info),
        .pNext = &rendering_info,
        .stageCount =
            static_cast<uint32_t>(m_stages.size()),
        .pStages = m_stages.data(),
        .pVertexInputState = &vertex_input_info,
        .pInputAssemblyState = &input_assembly_info,
        .pTessellationState = &tesselation_info,
        .pViewportState = &viewport_info,
        .pRasterizationState = &rasterization_info,
        .pMultisampleState = &multisample_info,
        .pDepthStencilState = &depth_stencil_info,
        .pColorBlendState = &color_blend_info,
        .pDynamicState = &dynamic_info,
        .layout = m_layout.layout,
    };

    VkPipeline pipeline = VK_NULL_HANDLE;
    vkCreateGraphicsPipelines(m_device, VK_NULL_HANDLE, 1, &create_info, nullptr, &pipeline);
    if (!pipeline) {
        throw std::runtime_error{"Vulkan: Failed to create graphics pipeline"};
    }

    return Pipeline{pipeline};
}

void GPB::verifyAndNormalize() {
    if (m_stages.hasVertex()) {
        assert(m_vertex_input_state and m_input_assembly_state);
    } else {
        m_vertex_input_state.reset();
        m_input_assembly_state.reset();
    }

    if (m_stages.hasTesselation()) {
        assert(m_tesselation_state);
    } else {
        m_tesselation_state.reset();
    }

    if (auto& vis = m_vertex_input_state) {
        bool dyn_stride = std::ranges::any_of(vis->bindings,
            [] (const auto& vib) {
                return vib.stride == VertexInputBinding::StrideDynamic;
            }
        );
        if (dyn_stride) {
            auto v = std::views::transform(vis->bindings,
                [] (auto& vib) -> unsigned& {
                    return vib.stride;
                }
            );
            std::ranges::fill(v, VertexInputBinding::StrideDynamic);
        }
    }

    if (const auto& [viewports, scissors] = m_viewport_state;
        not viewports.empty() and not scissors.empty()) {
        assert(viewports.size() == scissors.size());
    }

    if (m_rasterization_state) {
        assert(m_stages.hasFragment());
        assert(m_multisample_state);
    } else {
        m_depth_test_state.reset();
        m_stencil_test_state.reset();
        m_multisample_state.reset();
        m_color_state.reset();
    }

    if (auto& mss = m_multisample_state) {
        mss->sample_count =
            std::bit_ceil(mss->sample_count);
        mss->sample_count =
            std::min<unsigned>(mss->sample_count, VK_SAMPLE_COUNT_64_BIT);
    }

    if (auto& sts = m_stencil_test_state) {
        bool dyn_st_op      = false;
        bool dyn_cmp_mask   = false;
        bool dyn_write_mask = false;
        bool dyn_ref        = false;
        for (auto sosp: {&sts->front, &sts->back}) {
            auto& sos = *sosp;
            if (!dyn_st_op) {
                dyn_st_op = sos.ops.isDynamic();
            }
            if (!dyn_cmp_mask) {
                dyn_cmp_mask = sos.compare_mask.isDynamic();
            }
            if (!dyn_write_mask) {
                dyn_write_mask = sos.write_mask.isDynamic();
            }
            if (!dyn_ref) {
                dyn_ref = sos.reference.isDynamic();
            }
        }
        for (auto sosp: {&sts->front, &sts->back}) {
            auto& sos = *sosp;
            if (dyn_st_op) {
                sos.ops = PipelineStateDynamic;
            }
            if (dyn_cmp_mask) {
                sos.compare_mask = PipelineStateDynamic;
            }
            if (dyn_write_mask) {
                sos.write_mask = PipelineStateDynamic;
            }
            if (dyn_ref) {
                sos.reference = PipelineStateDynamic;
            }
        }
    }

    if (auto& cs = m_color_state) {
        auto bf_uses_constants = [] (VkBlendFactor f) {
            switch(f) {
                case VK_BLEND_FACTOR_CONSTANT_COLOR:
                case VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_COLOR:
                case VK_BLEND_FACTOR_CONSTANT_ALPHA:
                case VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_ALPHA:
                    return true;
                default:
                    return false;
            }
        };
        bool use_constants = std::ranges::any_of(
            cs->attachments.blend(), [&] (const auto& cba) {
                if (not cba.blendEnable) {
                    return false;
                }
                bool uses_constants =
                    bf_uses_constants(cba.srcColorBlendFactor) or
                    bf_uses_constants(cba.dstColorBlendFactor) or
                    bf_uses_constants(cba.srcAlphaBlendFactor) or
                    bf_uses_constants(cba.dstAlphaBlendFactor);
                return uses_constants; 
            }
        );
        if (use_constants) {
            assert(cs->blend_constants);
        } else {
            cs->blend_constants.reset();
        }
    }   
}
}
