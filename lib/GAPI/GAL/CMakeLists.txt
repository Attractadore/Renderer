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

set(GAL_API "Vulkan" CACHE STRING "The graphics API to use for the GAL backend")
if (GAL_API STREQUAL "Vulkan")
    add_subdirectory(Vulkan)
    target_link_libraries(GAL INTERFACE GAL_Vulkan)
else()
    message(FATAL_ERROR "Invalid GAL backend selected")
endif()

add_subdirectory(progs)
