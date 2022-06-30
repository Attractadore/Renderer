add_library(GAPIVulkan
    VulkanContext.cpp)
target_link_libraries(GAPIVulkan
    PUBLIC GAPIPublicInterface GAL
    PRIVATE GAPIPrivateInterface)
