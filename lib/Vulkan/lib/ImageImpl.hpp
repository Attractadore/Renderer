#pragma once
#include "Image.hpp"
#include "VKRAII.hpp"

namespace R1::VK {
struct ImageImpl {
    Vk::Image       image;
    Vk::Allocation  allocation;
};

VkImageCreateFlags ImageCapabilitiesToVK(const ImageCapabilities& caps);
VkImageUsageFlags  ImageUsageToVK(const ImageUsage& usage);
VkImageAspectFlags ImageAspectsToVK(const ImageAspects& aspects);
}

#include "ImageImpl.inl"
