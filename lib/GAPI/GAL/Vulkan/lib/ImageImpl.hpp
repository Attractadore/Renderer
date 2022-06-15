#pragma once
#include "GAL/Image.hpp"
#include "VKRAII.hpp"

namespace R1::GAL {
struct ImageImpl {
    Vk::Image       image;
    Vk::Allocation  allocation;
};

constexpr VkImageSubresourceRange ImageSubresourceRangeToVK(
    const ImageSubresourceRange& range
);
}

#include "ImageImpl.inl"
