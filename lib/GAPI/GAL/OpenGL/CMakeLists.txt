cmake_minimum_required(VERSION 3.14)
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake)

find_package(glad2 REQUIRED)
find_package(OpenGL COMPONENTS EGL REQUIRED)
find_package(X11 COMPONENTS xcb REQUIRED)

glad_add_library(GL46 MX LANGUAGE C API gl:core=4.6 EXTENSIONS NONE)

add_library(R1GLAPI
    GLInstance.hpp
    GLContext.hpp

    GLInstance.cpp
    GLContext.cpp
)
target_compile_features(R1GLAPI
    PUBLIC cxx_std_20
)
target_link_libraries(R1GLAPI
    PUBLIC Common OpenGL::EGL GL46 X11::X11 X11::xcb
)

add_subdirectory(progs)
