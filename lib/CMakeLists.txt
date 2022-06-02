add_subdirectory(Common)
add_subdirectory(Vulkan)
add_subdirectory(OpenGL)

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
target_compile_definitions(R1PrivateInterface
    INTERFACE R1_DEBUG=1
)

add_library(R1
    R1.cpp
    Scene.cpp
)
target_link_libraries(R1
    PUBLIC R1PublicInterface
    PRIVATE R1PrivateInterface
)

target_link_libraries(R1
    PRIVATE R1VKAPI
)
target_compile_definitions(R1
    PRIVATE R1_USE_VULKAN
)
