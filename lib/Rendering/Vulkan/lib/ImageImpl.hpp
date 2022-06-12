#pragma once
#include "Image.hpp"
#include "VKRAII.hpp"

namespace R1::VK {
struct ImageImpl {
    Vk::Image       image;
    Vk::Allocation  allocation;
};

constexpr VkImageSubresourceRange ImageSubresourceRangeToVK(
    const ImageSubresourceRange& range
);
}

#include "ImageImpl.inl"
