add_library(zerosugar_xr_data)

find_package(RecastNavigation REQUIRED)

target_link_libraries(zerosugar_xr_data PRIVATE
    zerosugar_xr_pch
    RecastNavigation::Detour
)

target_precompile_headers(zerosugar_xr_data REUSE_FROM zerosugar_xr_pch)

file(GLOB_RECURSE ZEROSUGAR_XR_DATA_SRC_FILES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/*.hpp
)

target_sources(zerosugar_xr_data PRIVATE ${ZEROSUGAR_XR_DATA_SRC_FILES})
