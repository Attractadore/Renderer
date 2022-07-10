add_subdirectory(GAPI)

find_package(glm REQUIRED)

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
    INTERFACE   ${PROJECT_SOURCE_DIR}/include/R1
                ${CMAKE_CURRENT_SOURCE_DIR}/)
target_link_libraries(R1PrivateInterface
    INTERFACE GAPI)

add_library(R1
    R1.cpp
    Scene.cpp)
target_link_libraries(R1
    PUBLIC R1PublicInterface
    PRIVATE R1PrivateInterface glm)

add_subdirectory(Vulkan)
