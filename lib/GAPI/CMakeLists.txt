project(GAPI)

add_library(Common INTERFACE)
target_compile_definitions(Common
    INTERFACE R1_DEBUG)
target_compile_features(Common
    INTERFACE cxx_std_20)
target_include_directories(Common
    INTERFACE include)

add_subdirectory(GAL)
add_subdirectory(lib)
