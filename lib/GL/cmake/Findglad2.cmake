cmake_minimum_required(VERSION 3.14)
if ($CACHE{glad2_FOUND})
    return()
endif()

include(FetchContent)

function(create_glad_deps)
    find_package(Python COMPONENTS Interpreter REQUIRED)
    set(packages_stamp ${glad2_SOURCE_DIR}/packages.stamp)
    set(requirements ${glad2_SOURCE_DIR}/requirements.txt)
    add_custom_command(
        OUTPUT ${packages_stamp}
        COMMAND Python::Interpreter -m pip install -r ${requirements} -t ${glad2_SOURCE_DIR} --upgrade &&
                ${CMAKE_COMMAND} -E touch ${packages_stamp}
    )
    add_custom_target(glad_deps
        DEPENDS ${packages_stamp}
    )
endfunction()

set(glad2_SOURCE https://github.com/Dav1dde/glad)
FetchContent_Declare(
    glad2
    GIT_REPOSITORY ${glad2_SOURCE}
    GIT_TAG glad2
    SOURCE_SUBDIR cmake
)
message(STATUS "Download glad2 from ${glad2_SOURCE}")
unset(glad2_SOURCE)
FetchContent_MakeAvailable(glad2)
message(STATUS "Download complete")
create_glad_deps()

function(glad_add_library)
    _glad_add_library(${ARGV})
    set(glad_library ${ARGV0})
    add_dependencies(${glad_library} glad_deps)
endfunction()

set(glad2_stamp TRUE)
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
    glad2
    REQUIRED_VARS glad2_stamp
)
unset(glad2_stamp)
