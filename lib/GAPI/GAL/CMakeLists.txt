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

function(add_target_if_found)
    if (TARGET ${ARGV1})
        add_library(${ARGV0} INTERFACE)
        target_include_directories(${ARGV0} INTERFACE include)
        target_link_libraries(${ARGV0} INTERFACE ${ARGV1})
    endif()
endfunction()

set(GAL_API "Vulkan" CACHE STRING "The graphics API to use for the GAL backend")
if (GAL_API STREQUAL "Vulkan")
    add_subdirectory(Vulkan)
    target_link_libraries(GAL INTERFACE GAL_Vulkan)
    add_target_if_found(GAL_SDL2 GAL_Vulkan_SDL2)
    add_target_if_found(GAL_Xlib GAL_Vulkan_Xlib)
    add_target_if_found(GAL_XCB GAL_Vulkan_XCB)
    add_target_if_found(GAL_Wayland GAL_Vulkan_Wayland)
    add_target_if_found(GAL_Win32 GAL_Vulkan_Win32)
elseif(GAL_API STREQUAL "OpenGL")
    add_subdirectory(OpenGL)
    target_link_libraries(GAL INTERFACE GAL_OpenGL)
    add_target_if_found(GAL_Xlib GAL_OpenGL_Xlib)
else()
    message(FATAL_ERROR "Invalid GAL backend selected")
endif()

add_subdirectory(progs)
