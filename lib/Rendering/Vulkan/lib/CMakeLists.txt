cmake_minimum_required(VERSION 3.16)

find_package(ECM REQUIRED)
list(APPEND CMAKE_MODULE_PATH ${ECM_FIND_MODULE_DIR})
find_package(Vulkan REQUIRED)
find_package(X11 COMPONENTS xcb REQUIRED)
find_package(Wayland COMPONENTS Client REQUIRED)

add_library(R1_VK_PublicInterface INTERFACE)
target_include_directories(R1_VK_PublicInterface
    INTERFACE ../include/
)

add_library(R1_VK_PrivateInterface INTERFACE)
target_include_directories(R1_VK_PrivateInterface
    INTERFACE ../include/Vulkan
)
target_link_libraries(R1_VK_PrivateInterface
    INTERFACE VulkanMemoryAllocator
)

add_library(R1_VK
    CommandImpl.hpp
    ContextImpl.hpp
    DeviceImpl.hpp
    ImageImpl.hpp
    InstanceImpl.hpp
    PipelineImpl.hpp
    SwapchainImpl.hpp
    VKRAII.hpp
    VKUtil.hpp

    CommandImpl.inl
    DeviceImpl.inl
    ImageImpl.inl
    PipelineImpl.inl
    SwapchainImpl.inl

    Command.cpp
    Context.cpp
    Image.cpp
    Instance.cpp
    Pipeline.cpp
    Queue.cpp
    Swapchain.cpp
    Sync.cpp
)
target_compile_features(R1_VK
    PUBLIC cxx_std_20
)
target_link_libraries(R1_VK
    PUBLIC Common Vulkan::Vulkan
    INTERFACE R1_VK_PublicInterface
    PRIVATE R1_VK_PrivateInterface
)
target_compile_definitions(R1_VK
    PUBLIC R1_USE_VULKAN
)

add_library(R1_VK_Xlib
    APIXlib.cpp
)
target_link_libraries(R1_VK_Xlib
    PUBLIC R1_VK X11::X11 
    PRIVATE R1_VK_PrivateInterface
)

add_library(R1_VK_XCB
    APIXCB.cpp
)
target_link_libraries(R1_VK_XCB
    PUBLIC R1_VK X11::xcb 
    PRIVATE R1_VK_PrivateInterface
)
