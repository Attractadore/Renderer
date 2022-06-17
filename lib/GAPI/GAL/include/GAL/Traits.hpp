#pragma once
#include "BaseTypes.hpp"

namespace R1::GAL::Detail {
struct Traits {
    using AttachmentLoadOp = GAL::AttachmentLoadOp;
    using AttachmentStoreOp = GAL::AttachmentStoreOp;
    using BlendFactor = GAL::BlendFactor;
    using BlendOp = GAL::BlendOp;
    using Buffer = GAL::Buffer;
    using ColorComponent = GAL::ColorComponent;
    using ColorComponentFlags = GAL::ColorComponentFlags;
    using ColorSpace = GAL::ColorSpace;
    using CommandBufferUsage = GAL::CommandBufferUsage;
    using CommandBufferUsageFlags = GAL::CommandBufferUsageFlags;
    using CommandPoolConfigFlags = GAL::CommandPoolConfigFlags;
    using CompareOp = GAL::CompareOp;
    using CompositeAlpha = GAL::CompositeAlpha;
    using CullMode = GAL::CullMode;
    using DeviceType = GAL::DeviceType;
    using Format = GAL::Format;
    using FrontFace = GAL::FrontFace;
    using GraphicsPipelineConfigs = GAL::GraphicsPipelineConfigs;
    using Image = GAL::Image;
    using ImageAspect = GAL::ImageAspect;
    using ImageAspectFlags = GAL::ImageAspectFlags;
    using ImageComponentSwizzle = GAL::ImageComponentSwizzle;
    using ImageConfigFlags = GAL::ImageConfigFlags;
    using ImageLayout = GAL::ImageLayout;
    using ImageType = GAL::ImageType;
    using ImageUsage = GAL::ImageUsage;
    using ImageUsageFlags = GAL::ImageUsageFlags;
    using ImageView = GAL::ImageView;
    using ImageViewType = GAL::ImageViewType;
    using LogicOp = GAL::LogicOp;
    using MemoryAccess = GAL::MemoryAccess;
    using MemoryAccessFlags = GAL::MemoryAccessFlags;
    using PipelineLayout = GAL::PipelineLayout;
    using PipelineStage = GAL::PipelineStage;
    using PipelineStageFlags = GAL::PipelineStageFlags;
    using PolygonMode = GAL::PolygonMode;
    using PresentMode = GAL::PresentMode;
    using PrimitiveTopology = GAL::PrimitiveTopology;
    using Queue = GAL::Queue;
    using QueueCapability = GAL::QueueCapability;
    using QueueCapabilityFlags = GAL::QueueCapabilityFlags;
    using RenderingConfigFlags = GAL::RenderingConfigFlags;
    using ResolveMode = GAL::ResolveMode;
    using ShaderModule = GAL::ShaderModule;
    using StencilOp = GAL::StencilOp;
    using VertexInputRate = GAL::VertexInputRate;
};
}
