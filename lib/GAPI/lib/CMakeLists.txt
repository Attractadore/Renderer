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

add_library(GAPI_Xlib INTERFACE)
target_link_libraries(GAPI_Xlib
    INTERFACE GAPIPublicInterface GAL_Xlib)

add_library(GAPI_XCB INTERFACE)
target_link_libraries(GAPI_XCB
    INTERFACE GAPIPublicInterface GAL_XCB)
