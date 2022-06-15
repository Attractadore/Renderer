project(GAL)

add_library(GAL_Common INTERFACE)
target_include_directories(GAL_Common
    INTERFACE include)
target_compile_definitions(GAL_Common
    INTERFACE cxx_std_20)
target_link_libraries(GAL_Common
    INTERFACE Common)

add_library(GAL INTERFACE)
target_include_directories(GAL INTERFACE include)
add_library(GAL_Xlib INTERFACE)
target_include_directories(GAL_Xlib INTERFACE include)
add_library(GAL_XCB INTERFACE)
target_include_directories(GAL_XCB INTERFACE include)

set(GAL_API "Vulkan")
if (GAL_API STREQUAL "Vulkan")
    add_subdirectory(Vulkan)
    target_link_libraries(GAL INTERFACE GAL_Vulkan)
    target_link_libraries(GAL_Xlib INTERFACE GAL_Vulkan_Xlib)
    target_link_libraries(GAL_XCB INTERFACE GAL_Vulkan_XCB)
else()
    message(FATAL_ERROR "No GAL backend selected")
endif()
