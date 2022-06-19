find_package(SDL2)
if (TARGET SDL2::SDL2)

if (TARGET GAL_SDL2)
    add_executable(GALListDevicesSDL2 GALListDevicesSDL2.cpp)
    target_link_libraries(GALListDevicesSDL2 GAL_SDL2)
    target_compile_definitions(GALListDevicesSDL2 PRIVATE SDL_MAIN_HANDLED)
    
    add_executable(GALCreateContextsSDL2 GALCreateContextsSDL2.cpp)
    target_link_libraries(GALCreateContextsSDL2 GAL_SDL2)
    target_compile_definitions(GALCreateContextsSDL2 PRIVATE SDL_MAIN_HANDLED)
endif()

if (TARGET GAL_Xlib)
    add_executable(GALListDevicesXlib GALListDevicesXlib.cpp)
    target_link_libraries(GALListDevicesXlib GAL_Xlib SDL2::SDL2)
    target_compile_definitions(GALListDevicesXlib PRIVATE SDL_MAIN_HANDLED)
    
    add_executable(GALCreateContextsXlib GALCreateContextsXlib.cpp)
    target_link_libraries(GALCreateContextsXlib GAL_Xlib SDL2::SDL2)
    target_compile_definitions(GALCreateContextsXlib PRIVATE SDL_MAIN_HANDLED)
endif()

find_package(X11 COMPONENTS X11_xcb)
if (TARGET GAL_XCB AND TARGET X11::X11_xcb)
    add_executable(GALListDevicesXCB GALListDevicesXCB.cpp)
    target_link_libraries(GALListDevicesXCB GAL_XCB X11::X11_xcb SDL2::SDL2)
    target_compile_definitions(GALListDevicesXCB PRIVATE SDL_MAIN_HANDLED)
    
    add_executable(GALCreateContextsXCB GALCreateContextsXCB.cpp)
    target_link_libraries(GALCreateContextsXCB GAL_XCB X11::X11_xcb SDL2::SDL2)
    target_compile_definitions(GALCreateContextsXCB PRIVATE SDL_MAIN_HANDLED)
endif()

if (TARGET GAL_Wayland)
    add_executable(GALListDevicesWayland GALListDevicesWayland.cpp)
    target_link_libraries(GALListDevicesWayland GAL_Wayland SDL2::SDL2)
    target_compile_definitions(GALListDevicesWayland PRIVATE SDL_MAIN_HANDLED)
    
    add_executable(GALCreateContextsWayland GALCreateContextsWayland.cpp)
    target_link_libraries(GALCreateContextsWayland GAL_Wayland SDL2::SDL2)
    target_compile_definitions(GALCreateContextsWayland PRIVATE SDL_MAIN_HANDLED)
endif()

if (TARGET GAL_Win32)
    add_executable(GALListDevicesWin32 GALListDevicesWin32.cpp)
    target_link_libraries(GALListDevicesWin32 GAL_Win32 SDL2::SDL2)
    target_compile_definitions(GALListDevicesWin32 PRIVATE SDL_MAIN_HANDLED)
    
    add_executable(GALCreateContextsWin32 GALCreateContextsWin32.cpp)
    target_link_libraries(GALCreateContextsWin32 GAL_Win32 SDL2::SDL2)
    target_compile_definitions(GALCreateContextsWin32 PRIVATE SDL_MAIN_HANDLED)
endif()

endif()
