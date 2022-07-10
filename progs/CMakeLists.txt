find_package(SDL2)
if (TARGET SDL2::SDL2)
    add_library(ProgOptions INTERFACE)
    target_link_libraries(ProgOptions INTERFACE R1 R1Vulkan SDL2::SDL2)
    target_compile_definitions(ProgOptions INTERFACE SDL_MAIN_HANDLED)
    target_compile_features(ProgOptions INTERFACE cxx_std_17)

    add_executable(DrawTriangle DrawTriangle.cpp)
    target_link_libraries(DrawTriangle ProgOptions)

    add_executable(DrawTriangles DrawTriangles.cpp)
    target_link_libraries(DrawTriangles ProgOptions)

    add_executable(DrawRotatingTriangle DrawRotatingTriangle.cpp)
    target_link_libraries(DrawRotatingTriangle ProgOptions)
endif()
