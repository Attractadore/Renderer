cmake_minimum_required(VERSION 3.15)

find_package(Boost COMPONENTS headers REQUIRED)
add_library(Common INTERFACE)
target_include_directories(Common INTERFACE include)
target_link_libraries(Common INTERFACE R1PrivateInterface Boost::headers)
target_compile_definitions(Common INTERFACE R1_DEBUG)
