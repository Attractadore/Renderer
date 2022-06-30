find_package(Vulkan REQUIRED)
add_subdirectory(generate)

set(VMA_STATIC_VULKAN_FUNCTIONS FALSE CACHE BOOL "" FORCE)
set(VMA_DYNAMIC_VULKAN_FUNCTIONS TRUE CACHE BOOL "" FORCE)

add_library(VKUtil INTERFACE
    VKUtil.hpp)
target_link_libraries(VKUtil
    INTERFACE   VKSType
                VKDispatchTable
                VKContextDispatcher
                VulkanMemoryAllocator)

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
    PUBLIC GAL_Common Vulkan::Vulkan
    PRIVATE VKUtil)
