project(GAPI)

find_package(Boost COMPONENTS container REQUIRED)
find_package(range-v3 REQUIRED)
add_library(Common INTERFACE)
target_compile_features(Common
    INTERFACE cxx_std_20)
target_include_directories(Common
    INTERFACE include)
target_link_libraries(Common
    INTERFACE Attractadore::SlotMap Boost::container range-v3::range-v3)
target_precompile_headers(Common
    INTERFACE
        <algorithm>
        <array>
        <chrono>
        <concepts>
        <filesystem>
        <fstream>
        <functional>
        <iostream>
        <memory>
        <queue>
        <span>
        <stdexcept>
        <string>
        <type_traits>
        <unordered_map>
        <utility>
        <vector>

        <boost/container/static_vector.hpp>
        <boost/container/small_vector.hpp>

        <range/v3/all.hpp>
)

set(DebugFeatures FALSE CACHE BOOL
    "Enable debug features such as graphics API validation")
if (DebugFeatures)
target_compile_definitions(Common
    INTERFACE R1_DEBUG)
endif()

add_subdirectory(GAL)
add_subdirectory(lib)
