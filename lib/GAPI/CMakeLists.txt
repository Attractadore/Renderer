project(GAPI)

add_library(Common INTERFACE)
target_compile_features(Common
    INTERFACE cxx_std_20)
target_include_directories(Common
    INTERFACE include)
target_link_libraries(Common
    INTERFACE Attractadore::SlotMap)

set(DebugFeatures FALSE CACHE BOOL
    "Enable debug features such as graphics API validation")
if (DebugFeatures)
target_compile_definitions(Common
    INTERFACE R1_DEBUG)
endif()

add_subdirectory(GAL)
add_subdirectory(lib)
