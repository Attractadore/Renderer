add_library(APICommon INTERFACE
    Types.hpp
)
target_include_directories(APICommon INTERFACE ..)
target_link_libraries(APICommon INTERFACE R1PrivateInterface)
