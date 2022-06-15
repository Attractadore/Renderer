add_library(GAPI
    Instance.cpp
    Context.cpp
    Swapchain.cpp
)
target_link_libraries(GAPI
    PUBLIC GAL 
)

add_library(GAPI_Xlib
    APIXlib.cpp
)
target_link_libraries(GAPI_Xlib
    PUBLIC GAL_Xlib 
)

add_library(GAPI_XCB
    APIXCB.cpp
)
target_link_libraries(GAPI_XCB
    PUBLIC GAL_XCB 
)
