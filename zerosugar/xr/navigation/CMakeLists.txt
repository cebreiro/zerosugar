add_library(zerosugar_xr_navigation)

find_package(RecastNavigation REQUIRED)
find_package(OpenGL REQUIRED)
find_package(SDL2 CONFIG REQUIRED)

target_link_libraries(zerosugar_xr_navigation PRIVATE
    zerosugar_xr_pch
    RecastNavigation::Detour
    RecastNavigation::DebugUtils
    SDL2::SDL2
    SDL2::SDL2main
    OpenGL::GL
    OpenGL::GLU
)

target_precompile_headers(zerosugar_xr_navigation REUSE_FROM zerosugar_xr_pch)

file(GLOB_RECURSE ZEROSUGAR_XR_NAVIGATION_SRC_FILES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/*.hpp
)

target_sources(zerosugar_xr_navigation PRIVATE ${ZEROSUGAR_XR_NAVIGATION_SRC_FILES})
