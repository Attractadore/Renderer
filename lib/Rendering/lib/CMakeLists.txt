add_library(R1Rendering_PublicInterface INTERFACE)
target_include_directories(R1Rendering_PublicInterface
    INTERFACE ${PROJECT_SOURCE_DIR}/include/
)

add_library(R1Rendering_PrivateInterface INTERFACE)
target_include_directories(R1Rendering_PrivateInterface
    INTERFACE ${PROJECT_SOURCE_DIR}/include/Rendering/
)

add_library(R1Rendering
    Instance.cpp
    Context.cpp
)
target_link_libraries(R1Rendering
    PUBLIC R1Rendering_PublicInterface R1_VK
    PRIVATE R1Rendering_PrivateInterface
)

add_library(R1RenderingXlib
    APIXlib.cpp
)
target_link_libraries(R1RenderingXlib
    PUBLIC R1Rendering_PublicInterface R1_VK_Xlib
    PRIVATE R1Rendering_PrivateInterface
)

add_library(R1RenderingXCB
    APIXCB.cpp
)
target_link_libraries(R1RenderingXCB
    PUBLIC R1Rendering_PublicInterface R1_VK_XCB
    PRIVATE R1Rendering_PrivateInterface
)
