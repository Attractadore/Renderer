add_library(GAPIPrivateInterface INTERFACE)
target_include_directories(GAPIPrivateInterface
    INTERFACE   ${PROJECT_SOURCE_DIR}/include/GAPI
                ${CMAKE_CURRENT_SOURCE_DIR})
add_library(GAPIPublicInterface INTERFACE)
target_include_directories(GAPIPublicInterface
    INTERFACE ${PROJECT_SOURCE_DIR}/include)

add_library(GAPI
    Instance.cpp
    Context.cpp)
target_link_libraries(GAPI
    PUBLIC GAPIPublicInterface GAL
    PRIVATE GAPIPrivateInterface)

add_subdirectory(Vulkan)
