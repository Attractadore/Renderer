add_library(GALProgTemplates INTERFACE)
target_include_directories(GALProgTemplates
    INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/Templates)
target_link_libraries(GALProgTemplates
    INTERFACE GAL)
