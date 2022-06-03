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
    PUBLIC Common Vulkan::Vulkan X11::xcb X11::X11
)

add_subdirectory(external)
add_subdirectory(lib)
add_subdirectory(progs)
