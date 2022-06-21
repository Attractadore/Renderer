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
set(GENERATE_DISPATCH_TABLE_PY ${CMAKE_CURRENT_LIST_DIR}/DispatchTable.py)
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
set(GENERATE_STYPE_PY ${CMAKE_CURRENT_LIST_DIR}/SType.py)
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

set(VULKAN_CONTEXT_DISPATCHER_HPP ${CMAKE_CURRENT_BINARY_DIR}/VKContextDispatcher/include/VKContextDispatcher.hpp)
set(GENERATE_CONTEXT_DISPATCHER_PY ${CMAKE_CURRENT_LIST_DIR}/ContextDispatcher.py)
add_custom_command(
    OUTPUT ${VULKAN_CONTEXT_DISPATCHER_HPP}
    DEPENDS ${VULKAN_SPEC_XML} ${GENERATE_CONTEXT_DISPATCHER_PY}
    COMMAND Python3::Interpreter ARGS
            ${GENERATE_CONTEXT_DISPATCHER_PY}
            ${VULKAN_SPEC_XML}
            ${VULKAN_CONTEXT_DISPATCHER_HPP}
    VERBATIM)
add_library(VKContextDispatcher INTERFACE
    ${VULKAN_CONTEXT_DISPATCHER_HPP})
get_filename_component(VKContextDispatcher_INCLUDE_DIR ${VULKAN_CONTEXT_DISPATCHER_HPP} DIRECTORY)
target_include_directories(VKContextDispatcher
    INTERFACE ${VKContextDispatcher_INCLUDE_DIR})
target_link_libraries(VKContextDispatcher
    INTERFACE Vulkan::Vulkan)
