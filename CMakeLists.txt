cmake_minimum_required(VERSION 3.16)
project(Renderer)

find_package(SDL2)
find_package(X11 COMPONENTS xcb)
find_package(ECM)
list(APPEND CMAKE_MODULE_PATH ${ECM_FIND_MODULE_DIR})
find_package(Wayland COMPONENTS Client)

add_subdirectory(lib)
add_subdirectory(progs)
