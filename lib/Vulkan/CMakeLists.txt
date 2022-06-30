cmake_minimum_required(VERSION 3.21)

find_package(Vulkan COMPONENTS Headers REQUIRED)
find_package(Boost COMPONENTS container REQUIRED)
add_library(R1VulkanImpl
    R1Vulkan.cpp
    VulkanScene.cpp
    VulkanSwapchain.cpp)
target_link_libraries(R1VulkanImpl
    PRIVATE R1PrivateInterface GAPIVulkan Boost::container)
target_link_libraries(R1 PUBLIC R1VulkanImpl)

add_library(R1Vulkan INTERFACE)
target_link_libraries(R1Vulkan
    INTERFACE R1 Vulkan::Headers)
