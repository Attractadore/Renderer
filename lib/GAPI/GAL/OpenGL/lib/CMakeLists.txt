find_package(glad2 REQUIRED)
find_package(OpenGL COMPONENTS EGL REQUIRED)

glad_add_library(GL46 MX LANGUAGE C API gl:core=4.6 EXTENSIONS NONE)

add_library(GAL_OpenGL
    Instance.cpp)
target_compile_definitions(GAL_OpenGL
    PUBLIC GAL_USE_OPENGL)
target_compile_features(GAL_OpenGL
    PUBLIC cxx_std_20)
target_include_directories(GAL_OpenGL
    PUBLIC ../include)
target_link_libraries(GAL_OpenGL
    PUBLIC GAL_Common GL46
    PRIVATE OpenGL::EGL)

if (TARGET X11::X11)
add_library(GAL_OpenGL_Xlib
    APIXlib.cpp)
target_link_libraries(GAL_OpenGL_Xlib
    PUBLIC GAL_OpenGL X11::X11)
endif()
