add_subdirectory(Rendering)

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
    PRIVATE R1PrivateInterface R1Rendering
)

add_library(R1Xlib
    R1Xlib.cpp
)
target_link_libraries(R1Xlib
    PUBLIC R1 R1PublicInterface
    PRIVATE R1PrivateInterface R1RenderingXlib
)

add_library(R1XCB
    R1XCB.cpp
)
target_link_libraries(R1XCB
    PUBLIC R1 R1PublicInterface
    PRIVATE R1PrivateInterface R1RenderingXCB
)
