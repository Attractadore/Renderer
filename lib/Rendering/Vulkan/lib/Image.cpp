#include "ImageImpl.hpp"
#include "VKUtil.hpp"
#include "ContextImpl.hpp"

namespace R1::VK {
Image CreateImage(Context ctx, const ImageConfig& config) {
    VkImageCreateInfo create_info = {
        .sType = sType(create_info),
        .flags = ImageCapabilitiesToVK(config.capabilities),
        .imageType = static_cast<VkImageType>(config.type),
        .format = static_cast<VkFormat>(config.format),
        .extent = {
            .width = config.width,
            .height = config.height,
            .depth = config.depth,
        },
        .mipLevels = config.mip_level_count,
        .arrayLayers = config.array_layer_count,
        .samples = static_cast<VkSampleCountFlagBits>(
            config.sample_count
        ),
        .tiling = VK_IMAGE_TILING_OPTIMAL,
        .usage = ImageUsageToVK(config.usage),
        .sharingMode = config.sharing_queue_families.empty() ?
            VK_SHARING_MODE_EXCLUSIVE: VK_SHARING_MODE_CONCURRENT,
        .queueFamilyIndexCount = static_cast<uint32_t>(
            config.sharing_queue_families.size()
        ),
        .pQueueFamilyIndices = reinterpret_cast<const unsigned*>(
            config.sharing_queue_families.data()
        ),
        .initialLayout = static_cast<VkImageLayout>(
            config.initial_layout
        ),
    };

    auto [alloc_usg, alloc_flags] = [] (ImageMemoryUsage mem_usg) ->
        std::tuple<VmaMemoryUsage, VmaAllocationCreateFlags> {
    using enum ImageMemoryUsage;
    switch(mem_usg) {
        case Default:
            return { VMA_MEMORY_USAGE_AUTO, 0};
        case RenderTarget:
            return {
                VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE, 
                VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT
            };
        case Swap:
            return { VMA_MEMORY_USAGE_AUTO_PREFER_HOST, 0 };
        default:
            assert(!"Unknow image memory usage");
    }} (config.memory_usage);

    VmaAllocationCreateInfo alloc_info = {
        .flags = alloc_flags,
        .usage = alloc_usg,
    };

    VkImage image = VK_NULL_HANDLE;
    VmaAllocation alloc = VK_NULL_HANDLE;
    vmaCreateImage(ctx->allocator.get(), &create_info, &alloc_info, &image, &alloc, nullptr);
    if (!image) {
        throw std::runtime_error{"Vulkan: Failed to create image"};
    }

    return std::make_unique<ImageImpl>(ImageImpl{
        .image{ctx->device.get(), image},
        .allocation{ctx->allocator.get(), alloc},
    }).release();
}

void DestroyImage(Context ctx, Image image) {
    delete image;
}

ImageView CreateImageView(Context ctx, Image image, const ImageViewConfig& config) {
    VkImageViewCreateInfo create_info = {
        .sType = sType(create_info),
        .image = image->image.get(),
        .viewType = static_cast<VkImageViewType>(config.type),
        .format = static_cast<VkFormat>(config.format),
        .components = {
            .r = static_cast<VkComponentSwizzle>(config.components.r),
            .g = static_cast<VkComponentSwizzle>(config.components.g),
            .b = static_cast<VkComponentSwizzle>(config.components.b),
            .a = static_cast<VkComponentSwizzle>(config.components.a),
        },
        .subresourceRange = ImageSubresourceRangeToVK(config.subresource_range),
    };

    VkImageView view = VK_NULL_HANDLE;
    vkCreateImageView(ctx->device.get(), &create_info, nullptr, &view);
    if (!view) {
        throw std::runtime_error{"Vulkan: Failed to create image view"};
    }

    return view;
}

void DestroyImageView(Context ctx, ImageView view) {
    vkDestroyImageView(ctx->device.get(), view, nullptr); 
}
}