cmake_minimum_required(VERSION 3.16)

find_package(ECM REQUIRED)
list(APPEND CMAKE_MODULE_PATH ${ECM_FIND_MODULE_DIR})
find_package(Vulkan REQUIRED)
find_package(X11 COMPONENTS xcb REQUIRED)
find_package(Wayland COMPONENTS Client REQUIRED)

add_library(GAL_Vulkan
    CommandImpl.hpp
    ContextImpl.hpp
    DeviceImpl.hpp
    ImageImpl.hpp
    InstanceImpl.hpp
    SwapchainImpl.hpp
    VKRAII.hpp
    VKUtil.hpp

    CommandImpl.inl
    DeviceImpl.inl
    ImageImpl.inl

    Command.cpp
    Context.cpp
    Image.cpp
    Instance.cpp
    Pipeline.cpp
    Queue.cpp
    Swapchain.cpp
    Sync.cpp
)
target_compile_definitions(GAL_Vulkan
    PUBLIC GAL_USE_VULKAN)
target_compile_features(GAL_Vulkan
    PUBLIC cxx_std_20)
target_include_directories(GAL_Vulkan
    PUBLIC ../include)
target_link_libraries(GAL_Vulkan
    PUBLIC GAL_Common Vulkan::Vulkan VulkanMemoryAllocator)

add_library(GAL_Vulkan_Xlib
    APIXlib.cpp)
target_link_libraries(GAL_Vulkan_Xlib
    PUBLIC GAL_Vulkan X11::X11)

add_library(GAL_Vulkan_XCB
    APIXCB.cpp)
target_link_libraries(GAL_Vulkan_XCB
    PUBLIC GAL_Vulkan X11::xcb)
