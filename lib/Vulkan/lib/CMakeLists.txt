cmake_minimum_required(VERSION 3.13)

find_package(Vulkan REQUIRED)

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
    VKRAII.hpp
    VKUtil.hpp

    CommandImpl.inl
    DeviceImpl.inl
    ImageImpl.inl
    PipelineImpl.inl

    Command.cpp
    Context.cpp
    Image.cpp
    Instance.cpp
    Pipeline.cpp
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

find_package(X11 COMPONENTS xcb)

if (TARGET X11::X11)
add_library(R1_VK_Xlib
    APIXlib.cpp
)
target_link_libraries(R1_VK_Xlib
    PUBLIC R1_VK X11::X11 
    PRIVATE R1_VK_PrivateInterface
)
endif()

if (TARGET X11::xcb)
add_library(R1_VK_XCB
    APIXCB.cpp
)
target_link_libraries(R1_VK_XCB
    PUBLIC R1_VK X11::xcb 
    PRIVATE R1_VK_PrivateInterface
)
endif()
