find_package(SDL2)

if (TARGET SDL2::SDL2)
    add_executable(DrawTriangle DrawTriangle.cpp)
    target_link_libraries(DrawTriangle R1 SDL2::SDL2)
endif()
