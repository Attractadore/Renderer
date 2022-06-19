cmake_minimum_required(VERSION 3.16)
find_package(SDL2)

if (TARGET SDL2::SDL2)
    # For wl_* in SDL_syswm
    find_package(ECM REQUIRED NO_MODULE)
    list(APPEND CMAKE_MODULE_PATH ${ECM_FIND_MODULE_DIR})
    find_package(Wayland COMPONENTS Client REQUIRED)

    add_executable(VKListDevices VKListDevices.cpp)
    target_link_libraries(VKListDevices GAL_Vulkan GAL_Vulkan_Xlib GAL_Vulkan_Wayland SDL2::SDL2)
    target_include_directories(VKListDevices PRIVATE ..)
    
    add_executable(VKCreateContexts VKCreateContexts.cpp)
    target_link_libraries(VKCreateContexts GAL_Vulkan GAL_Vulkan_Xlib GAL_Vulkan_Wayland SDL2::SDL2)
    target_include_directories(VKCreateContexts PRIVATE ..)
endif()
