find_package(Vulkan REQUIRED)

find_package(Python3 COMPONENTS Interpreter REQUIRED)
set(VULKAN_PREFIX ${Vulkan_INCLUDE_DIRS}/..)
message(STATUS "Find vk.xml")
set(VULKAN_SPEC_XML ${VULKAN_PREFIX}/share/vulkan/registry/vk.xml CACHE PATH "Path to Vulkan spec in XML format")
if (EXISTS ${VULKAN_SPEC_XML})
    message(STATUS "Found vk.xml")
else()
    set(VULKAN_SPEC_XML_URL https://raw.githubusercontent.com/KhronosGroup/Vulkan-Docs/main/xml/vk.xml)
    message(STATUS "Could NOT find vk.xml, dowload it from ${VULKAN_SPEC_XML_URL}:")
    set(VULKAN_SPEC_XML ${CMAKE_CURRENT_BINARY_DIR}/vk.xml CACHE PATH "" FORCE)
    file(DOWNLOAD ${VULKAN_SPEC_XML_URL} ${VULKAN_SPEC_XML})
    message(STATUS "Done")
endif()

set(VULKAN_DISPATCH_TABLE_H ${CMAKE_CURRENT_BINARY_DIR}/VKDispatchTable/include/VKDispatchTable.h)
set(VULKAN_DISPATCH_TABLE_C ${CMAKE_CURRENT_BINARY_DIR}/VKDispatchTable/lib/VKDispatchTable.c)
set(GENERATE_DISPATCH_TABLE_PY ${CMAKE_CURRENT_LIST_DIR}/GenerateDispatchTable.py)
add_custom_command(
    OUTPUT ${VULKAN_DISPATCH_TABLE_H} ${VULKAN_DISPATCH_TABLE_C}
    DEPENDS ${VULKAN_SPEC_XML} ${GENERATE_DISPATCH_TABLE_PY}
    COMMAND Python3::Interpreter ARGS
            ${GENERATE_DISPATCH_TABLE_PY}
            ${VULKAN_SPEC_XML}
            ${VULKAN_DISPATCH_TABLE_H}
            ${VULKAN_DISPATCH_TABLE_C}
    VERBATIM)
add_library(VKDispatchTable
    ${VULKAN_DISPATCH_TABLE_H}
    ${VULKAN_DISPATCH_TABLE_C})
get_filename_component(VKDispatchTable_INCLUDE_DIR ${VULKAN_DISPATCH_TABLE_H} DIRECTORY)
target_include_directories(VKDispatchTable
    PUBLIC ${VKDispatchTable_INCLUDE_DIR})
target_link_libraries(VKDispatchTable
    PUBLIC Vulkan::Vulkan)

set(VULKAN_STYPE_HPP ${CMAKE_CURRENT_BINARY_DIR}/VKSType/include/VKSType.hpp)
set(GENERATE_STYPE_PY ${CMAKE_CURRENT_LIST_DIR}/GenerateSType.py)
add_custom_command(
    OUTPUT ${VULKAN_STYPE_HPP}
    DEPENDS ${VULKAN_SPEC_XML} ${GENERATE_STYPE_PY}
    COMMAND Python3::Interpreter ARGS
            ${GENERATE_STYPE_PY}
            ${VULKAN_SPEC_XML}
            ${VULKAN_STYPE_HPP}
    VERBATIM)
add_library(VKSType INTERFACE ${VULKAN_STYPE_HPP})
get_filename_component(VKSType_INCLUDE_DIR ${VULKAN_STYPE_HPP} DIRECTORY)
target_include_directories(VKSType
    INTERFACE ${VKSType_INCLUDE_DIR})
target_link_libraries(VKSType
    INTERFACE Vulkan::Vulkan)

add_library(VKUtil INTERFACE
    VKUtil.hpp)
target_link_libraries(VKUtil
    INTERFACE VKSType)

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
    PRIVATE VKUtil VKDispatchTable)

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
