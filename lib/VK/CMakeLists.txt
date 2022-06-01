cmake_minimum_required(VERSION 3.13)

find_package(Vulkan REQUIRED)
find_package(X11 COMPONENTS xcb REQUIRED)

add_library(R1VKAPI
    VKContext.hpp
    VKImage.hpp
    VKInstance.hpp
    VKPipeline.hpp
    VKSwapchain.hpp

    VKContext.cpp
    VKImage.cpp
    VKInstance.cpp
    VKPipeline.cpp
    VKSwapchain.cpp
)
target_compile_features(R1VKAPI
    PUBLIC cxx_std_20
)
target_link_libraries(R1VKAPI
    PUBLIC APICommon Vulkan::Vulkan X11::xcb X11::X11
)

target_link_libraries(R1
    PRIVATE R1VKAPI
)

add_subdirectory(progs)
