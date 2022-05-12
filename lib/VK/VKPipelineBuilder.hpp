#pragma once
#include <vulkan/vulkan.h>

#include <array>
#include <functional>
#include <optional>
#include <variant>
#include <vector>

namespace R1::VK {
namespace Detail {
template<typename T>
struct VectorOf: private std::vector<T> {
    using Base = std::vector<T>;

    using Base::Base;
    using Base::size;
    using Base::empty;
    using Base::begin;
    using Base::end;
    using Base::data;
    using Base::operator[];
    using Base::clear;
};

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
    void map(StaticVisitor&& sv, DynamicOp&& op) const {
        if (not isDynamic()) {
            std::invoke(
                std::forward<StaticVisitor>(sv),
                std::get<T>(*this)
            );
        } else {
            std::invoke(std::forward<DynamicOp>(op));
        }
    }
};
}
inline constexpr auto PipelineStateDynamic =
    Detail::PipelineStateDynamicT::Enable;

enum class VertexInputRate {
    Vertex      = VK_VERTEX_INPUT_RATE_VERTEX,
    Instance    = VK_VERTEX_INPUT_RATE_INSTANCE,
};

struct VertexInputBinding {
    static constexpr unsigned StrideDynamic = -1;

    unsigned        binding;
    unsigned        stride;
    VertexInputRate input_rate = VertexInputRate::Vertex;
};

enum class VertexFormat {
    Float   = VK_FORMAT_R32_SFLOAT,
    Float2  = VK_FORMAT_R32G32_SFLOAT,
    Float3  = VK_FORMAT_R32G32B32_SFLOAT,
    Float4  = VK_FORMAT_R32G32B32A32_SFLOAT,
};

struct VertexInputAttribute {
    unsigned        location;
    unsigned        binding;
    VertexFormat    format;
    unsigned        offset;
};

class VertexInputBindingDescriptions:
    public Detail::VectorOf<VkVertexInputBindingDescription>
{
    using Base = Detail::VectorOf<VkVertexInputBindingDescription>;

public:
    VertexInputBindingDescriptions() = default;
    VertexInputBindingDescriptions(
        std::initializer_list<VertexInputBinding> list
    );
};

class VertexInputAttributeDescriptions:
    public Detail::VectorOf<VkVertexInputAttributeDescription>
{
    using Base = Detail::VectorOf<VkVertexInputAttributeDescription>;

public:
    VertexInputAttributeDescriptions() = default;
    VertexInputAttributeDescriptions(
        std::initializer_list<VertexInputAttribute> list
    );
};

struct VertexInputState {
    VertexInputBindingDescriptions bindings;
    VertexInputAttributeDescriptions attributes;
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
    Line                        = VK_PRIMITIVE_TOPOLOGY_LINE_LIST,
    Triangle                    = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
};

struct InputAssemblyState {
    std::variant<
        PrimitiveTopology, PrimitiveTopologyClass>  topology = PrimitiveTopology::TriangleList;
    Detail::PipelineState<bool>                     restart = false;
};

struct TessellationState {
    unsigned patch_control_point_count;
};

struct Viewport {
    float   x = 0.0f,
            y = 0.0f;
    float   width,
            height;
    float   min_depth = 0.0f,
            max_depth = 1.0f;
};

struct Scissor {
    int         x = 0,
                y = 0;
    unsigned    width,
                height;
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

struct DepthBiasState {
    bool                                dynamic = false;
    struct Values {
        float                           constant_factor,
                                        clamp = 0.0f,
                                        slope_factor;
    };
    Detail::PipelineState<Values>       values;
};

struct RasterizationState {
    bool                                dynamic_discard = false;
    PolygonMode                         polygon_mode    = PolygonMode::Fill;
    Detail::PipelineState<CullMode>     cull_mode       = CullMode::None;
    Detail::PipelineState<FrontFace>    front_face      = FrontFace::CounterClockwise;
    Detail::PipelineState<float>        line_width      = 1.0f;
    std::optional<DepthBiasState>       depth_bias;
};

struct SampleShading {
    float min;
};

struct MultisampleState {
    unsigned                        sample_count        = 1;
    std::optional<SampleShading>    sample_shading;
    std::optional<uint64_t>         sample_mask;
    bool                            alpha_to_coverage   = false;
    bool                            alpha_to_one        = false;
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

struct DepthBoundsTestState {
    bool                            dynamic = false;
    struct Bounds {
        float                       min = 0.0f,
                                    max = 1.0f;
    };
    Detail::PipelineState<Bounds>   bounds;
};

enum class DepthFormat {};

struct DepthTestState {
    bool                                dynamic             = false;
    Detail::PipelineState<bool>         write               = true;
    Detail::PipelineState<CompareOp>    compare_op          = CompareOp::Less;
    bool                                clamp               = false;
    std::optional<DepthBoundsTestState> bounds_test;
    DepthFormat                         attachment_format;
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

struct StencilOpState {
    struct Ops {
        StencilOp                   fail,
                                    pass,
                                    depth_fail;
        CompareOp                   compare;
    };
    Detail::PipelineState<Ops>      ops;
    Detail::PipelineState<uint32_t> compare_mask;
    Detail::PipelineState<uint32_t> write_mask;
    Detail::PipelineState<unsigned> reference;   
};

enum class StencilFormat {};

struct StencilTestState {
    bool            dynamic = false;
    StencilOpState  front,
                    back;
    StencilFormat   attachment_format;
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

enum class ColorFormat {
    RGB8_UNORM  = VK_FORMAT_R8G8B8_UNORM,
    RGBA8_UNORM = VK_FORMAT_R8G8B8A8_UNORM,
    BGR8_UNORM  = VK_FORMAT_B8G8R8_UNORM,
    BGRA8_UNORM = VK_FORMAT_B8G8R8A8_UNORM,
};

struct ColorComponentFlags {
    bool r: 1,
         g: 1,
         b: 1,
         a: 1;
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

struct BlendState {
    BlendFactor src_color_blend_factor;
    BlendFactor dst_color_blend_factor;
    BlendOp     color_blend_op;
    BlendFactor src_alpha_blend_factor;
    BlendFactor dst_alpha_blend_factor;
    BlendOp     alpha_blend_op;
};

struct ColorAttachment {
    ColorFormat                 format;
    ColorComponentFlags         color_mask = {1, 1, 1, 1};
    std::optional<BlendState>   blend_state;
};

class ColorAttachments {
    using ColorBlendAttachments = Detail::VectorOf<VkPipelineColorBlendAttachmentState>;
    using ColorAttachmentFormats = Detail::VectorOf<VkFormat>;

    ColorBlendAttachments   m_blend;
    ColorAttachmentFormats  m_format;

public:
    ColorAttachments(std::initializer_list<ColorAttachment> list);

    auto&       blend()       { return m_blend; }
    const auto& blend() const { return m_blend; }

    auto&       format()       { return m_format; }
    const auto& format() const { return m_format; }
};

struct ColorState {
    std::optional<LogicOp>                              blend_logic_op;
    ColorAttachments                                    attachments;
    std::optional<
        Detail::PipelineState<std::array<float, 4>>>    blend_constants;
};

struct ShaderModule {
    VkShaderModule module;
};

namespace Detail {
constexpr auto stage_count = std::size({
    VK_SHADER_STAGE_VERTEX_BIT,
    VK_SHADER_STAGE_FRAGMENT_BIT
});

class ShaderStages:
    std::array<VkPipelineShaderStageCreateInfo, Detail::stage_count>
{
    using Base = std::array<VkPipelineShaderStageCreateInfo, Detail::stage_count>;

    static constexpr auto c_stage_not_active = -1;

    unsigned m_stage_count = 0;
    unsigned m_vertex_stage = c_stage_not_active;
    unsigned m_fragment_stage = c_stage_not_active;

public:
    VkPipelineShaderStageCreateInfo&        vertex();
    const VkPipelineShaderStageCreateInfo&  vertex() const;
    bool                                    hasVertex() const;

    VkPipelineShaderStageCreateInfo&        fragment();
    const VkPipelineShaderStageCreateInfo&  fragment() const;
    bool                                    hasFragment() const;

    bool                                    hasTesselation() const;

    size_t size() const { return m_stage_count; }
    using Base::data;

private:
    unsigned                                index(unsigned idx);
    static bool                             hasStage(unsigned idx);
};
}

struct PipelineLayout {
    VkPipelineLayout layout;
};

struct Pipeline {
    VkPipeline pipeline;
};

class GraphicsPipelineBuilder {
    VkDevice m_device;

    PipelineLayout                      m_layout;

    Detail::ShaderStages                m_stages;

    // Vertex input
    std::optional<VertexInputState>     m_vertex_input_state;
    std::optional<InputAssemblyState>   m_input_assembly_state;

    // Pre-rasterization
    std::optional<TessellationState>    m_tesselation_state;
    std::optional<RasterizationState>   m_rasterization_state;

    // Fragment shader and fragment output
    std::optional<MultisampleState>     m_multisample_state;
    std::optional<DepthTestState>       m_depth_test_state;
    std::optional<StencilTestState>     m_stencil_test_state;
    std::optional<ColorState>           m_color_state;

public:
    GraphicsPipelineBuilder(VkDevice dev);

    GraphicsPipelineBuilder& setLayout(const PipelineLayout& layout);
    GraphicsPipelineBuilder& setVertexStage(
        const ShaderModule& shader_module,
        const char* entry_point = "main"
    );
    GraphicsPipelineBuilder& setFragmentStage(
        const ShaderModule& shader_module,
        const char* entry_point = "main"
    );
    GraphicsPipelineBuilder& setVertexInputState(
        VertexInputState vertex_input_state
    );
    GraphicsPipelineBuilder& setInputAssemblyState(
        const InputAssemblyState& input_assembly_state
    );
    GraphicsPipelineBuilder& setTessellationState(
        const TessellationState& tesselation_state
    );
    GraphicsPipelineBuilder& setRasterizationState(
        const RasterizationState& rasterization_state
    );
    GraphicsPipelineBuilder& setMultisampleState(
        const MultisampleState& multisample_state
    );
    GraphicsPipelineBuilder& setDepthTestState(
        const DepthTestState& depth_test_state
    );
    GraphicsPipelineBuilder& setStencilTestState(
        const StencilTestState& stencil_test_state
    );
    GraphicsPipelineBuilder& setColorState(
        ColorState color_state
    );

    Pipeline                 build();

private:
    void                     verifyAndNormalize();
};
}
