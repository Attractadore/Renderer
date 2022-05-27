cmake_minimum_required(VERSION 3.15)

find_package(Boost COMPONENTS headers REQUIRED)
add_library(APICommon INTERFACE
    Types.hpp
    Ref.hpp
    Util.hpp
)
target_include_directories(APICommon INTERFACE ..)
target_link_libraries(APICommon INTERFACE R1PrivateInterface Boost::headers)
