add_library(GAPIPrivateInterface INTERFACE)
target_include_directories(GAPIPrivateInterface
    INTERFACE ${PROJECT_SOURCE_DIR}/include/GAPI)
add_library(GAPIPublicInterface INTERFACE)
target_include_directories(GAPIPublicInterface
    INTERFACE ${PROJECT_SOURCE_DIR}/include)

add_library(GAPI
    Instance.cpp
    Context.cpp
    Swapchain.cpp)
target_link_libraries(GAPI
    PUBLIC GAPIPublicInterface GAL
    PRIVATE GAPIPrivateInterface)

if (TARGET GAL_SDL2)
add_library(GAPI_SDL2 INTERFACE)
target_link_libraries(GAPI_SDL2
    INTERFACE GAPIPublicInterface GAL_SDL2)
endif()

if (TARGET GAL_Xlib)
add_library(GAPI_Xlib INTERFACE)
target_link_libraries(GAPI_Xlib
    INTERFACE GAPIPublicInterface GAL_Xlib)
endif()

if (TARGET GAL_XCB)
add_library(GAPI_XCB INTERFACE)
target_link_libraries(GAPI_XCB
    INTERFACE GAPIPublicInterface GAL_XCB)
endif()

if (TARGET GAL_Wayland)
add_library(GAPI_Wayland INTERFACE)
target_link_libraries(GAPI_Wayland
    INTERFACE GAPIPublicInterface GAL_Wayland)
endif()

if (TARGET GAL_Win32)
add_library(GAPI_Win32 INTERFACE)
target_link_libraries(GAPI_Win32
    INTERFACE GAPIPublicInterface GAL_Win32)
endif()
