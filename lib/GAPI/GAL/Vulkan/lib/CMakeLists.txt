find_package(Vulkan REQUIRED)
add_subdirectory(generate)

add_library(VKUtil INTERFACE
    VKUtil.hpp)
target_link_libraries(VKUtil
    INTERFACE VKSType VKDispatchTable VKContextDispatcher)

add_library(GAL_Vulkan
    CommandImpl.hpp
    ContextImpl.hpp
    DeviceImpl.hpp
    ImageImpl.hpp
    InstanceImpl.hpp
    SwapchainImpl.hpp
    VKRAII.hpp

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
    PUBLIC GAL_Common Vulkan::Vulkan VulkanMemoryAllocator
    PRIVATE VKUtil)

if (TARGET SDL2::SDL2)
add_library(GAL_Vulkan_SDL2
    APISDL2.cpp)
target_link_libraries(GAL_Vulkan_SDL2
    PUBLIC GAL_Vulkan SDL2::SDL2
    PRIVATE VKUtil)
endif()

if (TARGET X11::X11)
add_library(GAL_Vulkan_Xlib
    APIXlib.cpp)
target_link_libraries(GAL_Vulkan_Xlib
    PUBLIC GAL_Vulkan X11::X11)
endif()

if (TARGET X11::xcb)
add_library(GAL_Vulkan_XCB
    APIXCB.cpp)
target_link_libraries(GAL_Vulkan_XCB
    PUBLIC GAL_Vulkan X11::xcb)
endif()

if (TARGET Wayland::Client)
add_library(GAL_Vulkan_Wayland
    APIWayland.cpp)
target_link_libraries(GAL_Vulkan_Wayland
    PUBLIC GAL_Vulkan Wayland::Client
    PRIVATE VKUtil)
endif()

if (WIN32)
add_library(GAL_Vulkan_Win32
    APIWin32.cpp)
target_link_libraries(GAL_Vulkan_Win32
    PUBLIC GAL_Vulkan
    PRIVATE VKUtil)
endif()
