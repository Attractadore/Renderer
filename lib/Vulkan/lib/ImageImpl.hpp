#pragma once
#include "Image.hpp"
#include "VKRAII.hpp"

namespace R1::VK {
struct ImageImpl {
    Vk::Image       image;
    Vk::Allocation  allocation;
};

constexpr VkImageCreateFlags ImageCapabilitiesToVK(
    const ImageCapabilities& caps
);
constexpr VkImageUsageFlags  ImageUsageToVK(
    const ImageUsage& usage
);
constexpr VkImageAspectFlags ImageAspectsToVK(
    const ImageAspects& aspects
);
constexpr VkImageSubresourceRange ImageSubresourceRangeToVK(
    const ImageSubresourceRange& range
);
}

#include "ImageImpl.inl"
