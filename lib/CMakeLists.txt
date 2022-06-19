add_subdirectory(GAPI)

set(R1PublicHeaders
    ${PROJECT_SOURCE_DIR}/include/R1/R1Types.h
    ${PROJECT_SOURCE_DIR}/include/R1/R1.h
)

add_library(R1PublicInterface INTERFACE
    ${R1PublicHeaders}
)
target_include_directories(R1PublicInterface 
    INTERFACE ${PROJECT_SOURCE_DIR}/include/
)

add_library(R1PrivateInterface INTERFACE
    ${R1PublicHeaders}
)
target_include_directories(R1PrivateInterface 
    INTERFACE ${PROJECT_SOURCE_DIR}/include/R1
)

add_library(R1
    R1.cpp
    Scene.cpp
    Swapchain.cpp
)
target_link_libraries(R1
    PUBLIC R1PublicInterface
    PRIVATE R1PrivateInterface GAPI
)

if (TARGET GAPI_SDL2)
add_library(R1SDL2
    R1SDL2.cpp
)
target_link_libraries(R1SDL2
    PUBLIC R1 R1PublicInterface SDL2::SDL2
    PRIVATE R1PrivateInterface GAPI_SDL2
)
endif()

if (TARGET GAPI_Xlib)
add_library(R1Xlib
    R1Xlib.cpp
)
target_link_libraries(R1Xlib
    PUBLIC R1 R1PublicInterface X11::X11
    PRIVATE R1PrivateInterface GAPI_Xlib 
)
endif()

if (TARGET GAPI_XCB)
add_library(R1XCB
    R1XCB.cpp
)
target_link_libraries(R1XCB
    PUBLIC R1 R1PublicInterface X11::xcb
    PRIVATE R1PrivateInterface GAPI_XCB 
)
endif()

if (TARGET GAPI_Wayland)
add_library(R1Wayland
    R1Wayland.cpp
)
target_link_libraries(R1Wayland
    PUBLIC R1 R1PublicInterface Wayland::Client
    PRIVATE R1PrivateInterface GAPI_Wayland
)
endif()
