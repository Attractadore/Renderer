#pragma once
#include "GAL/Image.hpp"

#include <vk_mem_alloc.h>

namespace R1::GAL {
struct ImageImpl {
    VkImage image;
};

struct ImageWithAllocation: ImageImpl {
    VmaAllocation allocation;
};

constexpr VkImageSubresourceRange ImageSubresourceRangeToVK(
    const ImageSubresourceRange& range
);
}

#include "ImageImpl.inl"
